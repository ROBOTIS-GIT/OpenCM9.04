/*
 *  drv_eeprom.c
 *
 *  Created on: 2016. 7. 13.
 *      Author: Baram, PBHP
 */

#include "drv_eeprom.h"
#include "variant.h"





static bool IsInit = false;





/* Global variable used to store variable value in read sequence */
uint16_t DataVar = 0;



/* Variables' number */
#define NB_OF_VAR             (512)



/* Virtual address defined by the user: 0xFFFF value is prohibited */
static uint16_t VirtAddVarTab[NB_OF_VAR];


uint16_t EE_Init(void);
uint16_t EE_ReadVariable(uint16_t VirtAddress, uint16_t* Data);
uint16_t EE_WriteVariable(uint16_t VirtAddress, uint16_t Data);





int drv_eeprom_init()
{
  uint16_t i;


  for( i=0; i<NB_OF_VAR; i++ )
  {
    VirtAddVarTab[i] = i;
  }


  HAL_FLASH_Unlock();

  if( EE_Init() == HAL_OK )
  {
    IsInit = true;
  }
  return 0;
}



uint8_t drv_eeprom_read_byte(int addr)
{
  uint16_t read_value;


  if( IsInit == false ) return 0;

  EE_ReadVariable((uint16_t)addr,  &read_value);

  return (uint8_t)read_value;
}


void drv_eeprom_write_byte(int index, uint8_t data_in)
{
  if( IsInit == false ) return;

  EE_WriteVariable(index, (uint16_t)data_in);
}


uint16_t drv_eeprom_get_length(void)
{
  if( IsInit == false ) return 0;

  return NB_OF_VAR;
}



#define ADDR_FLASH_PAGE_126   ((uint32_t)0x0801F800) /* Base @ of Page 126, 1 Kbytes */
#define ADDR_FLASH_PAGE_127   ((uint32_t)0x0801FC00) /* Base @ of Page 127, 1 Kbytes */



/* Define the size of the sectors to be used */
#define PAGE_SIZE               (uint32_t)FLASH_PAGE_SIZE  /* Page size */

/* EEPROM start address in Flash */
#define EEPROM_START_ADDRESS  ((uint32_t)ADDR_FLASH_PAGE_126) /* EEPROM emulation start address */

/* Pages 0 and 1 base and end addresses */
#define PAGE0_BASE_ADDRESS    ((uint32_t)(EEPROM_START_ADDRESS + 0x0000))
#define PAGE0_END_ADDRESS     ((uint32_t)(EEPROM_START_ADDRESS + (PAGE_SIZE - 1)))
#define PAGE0_ID               ADDR_FLASH_PAGE_126

#define PAGE1_BASE_ADDRESS    ((uint32_t)(EEPROM_START_ADDRESS + PAGE_SIZE))
#define PAGE1_END_ADDRESS     ((uint32_t)(EEPROM_START_ADDRESS + PAGE_SIZE + PAGE_SIZE - 1))
#define PAGE1_ID               ADDR_FLASH_PAGE_127

/* Used Flash pages for EEPROM emulation */
#define PAGE0                 ((uint16_t)0x0000)
#define PAGE1                 ((uint16_t)0x0040)

/* No valid page define */
#define NO_VALID_PAGE         ((uint16_t)0x00AB)

/* Page status definitions */
#define ERASED                ((uint16_t)0xFFFF)     /* Page is empty */
#define RECEIVE_DATA          ((uint16_t)0xEEEE)     /* Page is marked to receive data */
#define VALID_PAGE            ((uint16_t)0x0000)     /* Page containing valid data */

/* Valid pages in read and write defines */
#define READ_FROM_VALID_PAGE  ((uint8_t)0x00)
#define WRITE_IN_VALID_PAGE   ((uint8_t)0x01)

/* Page full define */
#define PAGE_FULL             ((uint8_t)0x80)



/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static HAL_StatusTypeDef EE_Format(void);
static uint16_t EE_FindValidPage(uint8_t Operation);
static uint16_t EE_VerifyPageFullWriteVariable(uint16_t VirtAddress, uint16_t Data);
static uint16_t EE_PageTransfer(uint16_t VirtAddress, uint16_t Data);
static uint16_t EE_VerifyPageFullyErased(uint32_t Address);



/**
  * @brief  Restore the pages to a known good state in case of page's status
  *   corruption after a power loss.
  * @param  None.
  * @retval - Flash error code: on write Flash error
  *         - FLASH_COMPLETE: on success
  */
uint16_t EE_Init(void)
{
  uint16_t pagestatus0 = 6, pagestatus1 = 6;
  uint16_t varidx = 0;
  uint16_t eepromstatus = 0, readstatus = 0;
  int16_t x = -1;
  HAL_StatusTypeDef  flashstatus;
  uint32_t page_error = 0;
  FLASH_EraseInitTypeDef s_eraseinit;


  /* Get Page0 status */
  pagestatus0 = (*(__IO uint16_t*)PAGE0_BASE_ADDRESS);
  /* Get Page1 status */
  pagestatus1 = (*(__IO uint16_t*)PAGE1_BASE_ADDRESS);

  /* Fill EraseInit structure*/
  s_eraseinit.TypeErase   = FLASH_TYPEERASE_PAGES;
  s_eraseinit.PageAddress = PAGE0_ID;
  s_eraseinit.NbPages     = 1;

  /* Check for invalid header states and repair if necessary */
  switch (pagestatus0)
  {
    case ERASED:
      if (pagestatus1 == VALID_PAGE) /* Page0 erased, Page1 valid */
      {
          /* Erase Page0 */
        if(!EE_VerifyPageFullyErased(PAGE0_BASE_ADDRESS))
        {
          flashstatus = HAL_FLASHEx_Erase(&s_eraseinit, &page_error);
          /* If erase operation was failed, a Flash error code is returned */
          if (flashstatus != HAL_OK)
          {
            return flashstatus;
          }
        }
      }
      else if (pagestatus1 == RECEIVE_DATA) /* Page0 erased, Page1 receive */
      {
        /* Erase Page0 */
        if(!EE_VerifyPageFullyErased(PAGE0_BASE_ADDRESS))
        {
          flashstatus = HAL_FLASHEx_Erase(&s_eraseinit, &page_error);
          /* If erase operation was failed, a Flash error code is returned */
          if (flashstatus != HAL_OK)
          {
            return flashstatus;
          }
        }
        /* Mark Page1 as valid */
        flashstatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, PAGE1_BASE_ADDRESS, VALID_PAGE);
        /* If program operation was failed, a Flash error code is returned */
        if (flashstatus != HAL_OK)
        {
          return flashstatus;
        }
      }
      else /* First EEPROM access (Page0&1 are erased) or invalid state -> format EEPROM */
      {
        /* Erase both Page0 and Page1 and set Page0 as valid page */
        flashstatus = EE_Format();
        /* If erase/program operation was failed, a Flash error code is returned */
        if (flashstatus != HAL_OK)
        {
          return flashstatus;
        }
      }
      break;

    case RECEIVE_DATA:
      if (pagestatus1 == VALID_PAGE) /* Page0 receive, Page1 valid */
      {
        /* Transfer data from Page1 to Page0 */
        for (varidx = 0; varidx < NB_OF_VAR; varidx++)
        {
          if (( *(__IO uint16_t*)(PAGE0_BASE_ADDRESS + 6)) == VirtAddVarTab[varidx])
          {
            x = varidx;
          }
          if (varidx != x)
          {
            /* Read the last variables' updates */
            readstatus = EE_ReadVariable(VirtAddVarTab[varidx], &DataVar);
            /* In case variable corresponding to the virtual address was found */
            if (readstatus != 0x1)
            {
              /* Transfer the variable to the Page0 */
              eepromstatus = EE_VerifyPageFullWriteVariable(VirtAddVarTab[varidx], DataVar);
              /* If program operation was failed, a Flash error code is returned */
              if (eepromstatus != HAL_OK)
              {
                return eepromstatus;
              }
            }
          }
        }
        /* Mark Page0 as valid */
        flashstatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, PAGE0_BASE_ADDRESS, VALID_PAGE);
        /* If program operation was failed, a Flash error code is returned */
        if (flashstatus != HAL_OK)
        {
          return flashstatus;
        }
        s_eraseinit.TypeErase   = FLASH_TYPEERASE_PAGES;
        s_eraseinit.PageAddress = PAGE1_ID;
        s_eraseinit.NbPages     = 1;
        /* Erase Page1 */
        if(!EE_VerifyPageFullyErased(PAGE1_BASE_ADDRESS))
        {
          flashstatus = HAL_FLASHEx_Erase(&s_eraseinit, &page_error);
          /* If erase operation was failed, a Flash error code is returned */
          if (flashstatus != HAL_OK)
          {
            return flashstatus;
          }
        }
      }
      else if (pagestatus1 == ERASED) /* Page0 receive, Page1 erased */
      {
        s_eraseinit.TypeErase   = FLASH_TYPEERASE_PAGES;
        s_eraseinit.PageAddress = PAGE1_ID;
        s_eraseinit.NbPages     = 1;
        /* Erase Page1 */
        if(!EE_VerifyPageFullyErased(PAGE1_BASE_ADDRESS))
        {
          flashstatus = HAL_FLASHEx_Erase(&s_eraseinit, &page_error);
          /* If erase operation was failed, a Flash error code is returned */
          if (flashstatus != HAL_OK)
          {
            return flashstatus;
          }
        }
        /* Mark Page0 as valid */
        flashstatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, PAGE0_BASE_ADDRESS, VALID_PAGE);
        /* If program operation was failed, a Flash error code is returned */
        if (flashstatus != HAL_OK)
        {
          return flashstatus;
        }
      }
      else /* Invalid state -> format eeprom */
      {
        /* Erase both Page0 and Page1 and set Page0 as valid page */
        flashstatus = EE_Format();
        /* If erase/program operation was failed, a Flash error code is returned */
        if (flashstatus != HAL_OK)
        {
          return flashstatus;
        }
      }
      break;

    case VALID_PAGE:
      if (pagestatus1 == VALID_PAGE) /* Invalid state -> format eeprom */
      {
        /* Erase both Page0 and Page1 and set Page0 as valid page */
        flashstatus = EE_Format();
        /* If erase/program operation was failed, a Flash error code is returned */
        if (flashstatus != HAL_OK)
        {
          return flashstatus;
        }
      }
      else if (pagestatus1 == ERASED) /* Page0 valid, Page1 erased */
      {
        s_eraseinit.TypeErase   = FLASH_TYPEERASE_PAGES;
        s_eraseinit.PageAddress = PAGE1_ID;
        s_eraseinit.NbPages     = 1;
        /* Erase Page1 */
        if(!EE_VerifyPageFullyErased(PAGE1_BASE_ADDRESS))
        {
          flashstatus = HAL_FLASHEx_Erase(&s_eraseinit, &page_error);
          /* If erase operation was failed, a Flash error code is returned */
          if (flashstatus != HAL_OK)
          {
            return flashstatus;
          }
        }
      }
      else /* Page0 valid, Page1 receive */
      {
        /* Transfer data from Page0 to Page1 */
        for (varidx = 0; varidx < NB_OF_VAR; varidx++)
        {
          if ((*(__IO uint16_t*)(PAGE1_BASE_ADDRESS + 6)) == VirtAddVarTab[varidx])
          {
            x = varidx;
          }
          if (varidx != x)
          {
            /* Read the last variables' updates */
            readstatus = EE_ReadVariable(VirtAddVarTab[varidx], &DataVar);
            /* In case variable corresponding to the virtual address was found */
            if (readstatus != 0x1)
            {
              /* Transfer the variable to the Page1 */
              eepromstatus = EE_VerifyPageFullWriteVariable(VirtAddVarTab[varidx], DataVar);
              /* If program operation was failed, a Flash error code is returned */
              if (eepromstatus != HAL_OK)
              {
                return eepromstatus;
              }
            }
          }
        }
        /* Mark Page1 as valid */
        flashstatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, PAGE1_BASE_ADDRESS, VALID_PAGE);
        /* If program operation was failed, a Flash error code is returned */
        if (flashstatus != HAL_OK)
        {
          return flashstatus;
        }
        s_eraseinit.TypeErase   = FLASH_TYPEERASE_PAGES;
        s_eraseinit.PageAddress = PAGE0_ID;
        s_eraseinit.NbPages     = 1;
        /* Erase Page0 */
        if(!EE_VerifyPageFullyErased(PAGE0_BASE_ADDRESS))
        {
          flashstatus = HAL_FLASHEx_Erase(&s_eraseinit, &page_error);
          /* If erase operation was failed, a Flash error code is returned */
          if (flashstatus != HAL_OK)
          {
            return flashstatus;
          }
        }
      }
      break;

    default:  /* Any other state -> format eeprom */
      /* Erase both Page0 and Page1 and set Page0 as valid page */
      flashstatus = EE_Format();
      /* If erase/program operation was failed, a Flash error code is returned */
      if (flashstatus != HAL_OK)
      {
        return flashstatus;
      }
      break;
  }

  return HAL_OK;
}

/**
  * @brief  Verify if specified page is fully erased.
  * @param  Address: page address
  *   This parameter can be one of the following values:
  *     @arg PAGE0_BASE_ADDRESS: Page0 base address
  *     @arg PAGE1_BASE_ADDRESS: Page1 base address
  * @retval page fully erased status:
  *           - 0: if Page not erased
  *           - 1: if Page erased
  */
uint16_t EE_VerifyPageFullyErased(uint32_t Address)
{
  uint32_t readstatus = 1;
  uint16_t addressvalue = 0x5555;

  /* Check each active page address starting from end */
  while (Address <= PAGE0_END_ADDRESS)
  {
    /* Get the current location content to be compared with virtual address */
    addressvalue = (*(__IO uint16_t*)Address);

    /* Compare the read address with the virtual address */
    if (addressvalue != ERASED)
    {

      /* In case variable value is read, reset readstatus flag */
      readstatus = 0;

      break;
    }
    /* Next address location */
    Address = Address + 4;
  }

  /* Return readstatus value: (0: Page not erased, 1: Page erased) */
  return readstatus;
}

/**
  * @brief  Returns the last stored variable data, if found, which correspond to
  *   the passed virtual address
  * @param  VirtAddress: Variable virtual address
  * @param  Data: Global variable contains the read variable value
  * @retval Success or error status:
  *           - 0: if variable was found
  *           - 1: if the variable was not found
  *           - NO_VALID_PAGE: if no valid page was found.
  */
uint16_t EE_ReadVariable(uint16_t VirtAddress, uint16_t* Data)
{
  uint16_t validpage = PAGE0;
  uint16_t addressvalue = 0x5555, readstatus = 1;
  uint32_t address = EEPROM_START_ADDRESS, PageStartAddress = EEPROM_START_ADDRESS;

  /* Get active Page for read operation */
  validpage = EE_FindValidPage(READ_FROM_VALID_PAGE);

  /* Check if there is no valid page */
  if (validpage == NO_VALID_PAGE)
  {
    return  NO_VALID_PAGE;
  }

  /* Get the valid Page start Address */
  PageStartAddress = (uint32_t)(EEPROM_START_ADDRESS + (uint32_t)(validpage * PAGE_SIZE));

  /* Get the valid Page end Address */
  address = (uint32_t)((EEPROM_START_ADDRESS - 2) + (uint32_t)((1 + validpage) * PAGE_SIZE));

  /* Check each active page address starting from end */
  while (address > (PageStartAddress + 2))
  {
    /* Get the current location content to be compared with virtual address */
    addressvalue = (*(__IO uint16_t*)address);

    /* Compare the read address with the virtual address */
    if (addressvalue == VirtAddress)
    {
      /* Get content of Address-2 which is variable value */
      *Data = (*(__IO uint16_t*)(address - 2));

      /* In case variable value is read, reset readstatus flag */
      readstatus = 0;

      break;
    }
    else
    {
      /* Next address location */
      address = address - 4;
    }
  }

  /* Return readstatus value: (0: variable exist, 1: variable doesn't exist) */
  return readstatus;
}

/**
  * @brief  Writes/upadtes variable data in EEPROM.
  * @param  VirtAddress: Variable virtual address
  * @param  Data: 16 bit data to be written
  * @retval Success or error status:
  *           - FLASH_COMPLETE: on success
  *           - PAGE_FULL: if valid page is full
  *           - NO_VALID_PAGE: if no valid page was found
  *           - Flash error code: on write Flash error
  */
uint16_t EE_WriteVariable(uint16_t VirtAddress, uint16_t Data)
{
  uint16_t Status = 0;

  /* Write the variable virtual address and value in the EEPROM */
  Status = EE_VerifyPageFullWriteVariable(VirtAddress, Data);

  /* In case the EEPROM active page is full */
  if (Status == PAGE_FULL)
  {
    /* Perform Page transfer */
    Status = EE_PageTransfer(VirtAddress, Data);
  }

  /* Return last operation status */
  return Status;
}

/**
  * @brief  Erases PAGE and PAGE1 and writes VALID_PAGE header to PAGE
  * @param  None
  * @retval Status of the last operation (Flash write or erase) done during
  *         EEPROM formating
  */
static HAL_StatusTypeDef EE_Format(void)
{
  HAL_StatusTypeDef flashstatus = HAL_OK;
  uint32_t page_error = 0;
  FLASH_EraseInitTypeDef s_eraseinit;

  s_eraseinit.TypeErase   = FLASH_TYPEERASE_PAGES;
  s_eraseinit.PageAddress = PAGE0_ID;
  s_eraseinit.NbPages     = 1;
  /* Erase Page0 */
  if(!EE_VerifyPageFullyErased(PAGE0_BASE_ADDRESS))
  {
    flashstatus = HAL_FLASHEx_Erase(&s_eraseinit, &page_error);
    /* If erase operation was failed, a Flash error code is returned */
    if (flashstatus != HAL_OK)
    {
      return flashstatus;
    }
  }
  /* Set Page0 as valid page: Write VALID_PAGE at Page0 base address */
  flashstatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, PAGE0_BASE_ADDRESS, VALID_PAGE);
  /* If program operation was failed, a Flash error code is returned */
  if (flashstatus != HAL_OK)
  {
    return flashstatus;
  }

  s_eraseinit.PageAddress = PAGE1_ID;
  /* Erase Page1 */
  if(!EE_VerifyPageFullyErased(PAGE1_BASE_ADDRESS))
  {
    flashstatus = HAL_FLASHEx_Erase(&s_eraseinit, &page_error);
    /* If erase operation was failed, a Flash error code is returned */
    if (flashstatus != HAL_OK)
    {
      return flashstatus;
    }
  }

  return HAL_OK;
}

/**
  * @brief  Find valid Page for write or read operation
  * @param  Operation: operation to achieve on the valid page.
  *   This parameter can be one of the following values:
  *     @arg READ_FROM_VALID_PAGE: read operation from valid page
  *     @arg WRITE_IN_VALID_PAGE: write operation from valid page
  * @retval Valid page number (PAGE or PAGE1) or NO_VALID_PAGE in case
  *   of no valid page was found
  */
static uint16_t EE_FindValidPage(uint8_t Operation)
{
  uint16_t pagestatus0 = 6, pagestatus1 = 6;

  /* Get Page0 actual status */
  pagestatus0 = (*(__IO uint16_t*)PAGE0_BASE_ADDRESS);

  /* Get Page1 actual status */
  pagestatus1 = (*(__IO uint16_t*)PAGE1_BASE_ADDRESS);

  /* Write or read operation */
  switch (Operation)
  {
    case WRITE_IN_VALID_PAGE:   /* ---- Write operation ---- */
      if (pagestatus1 == VALID_PAGE)
      {
        /* Page0 receiving data */
        if (pagestatus0 == RECEIVE_DATA)
        {
          return PAGE0;         /* Page0 valid */
        }
        else
        {
          return PAGE1;         /* Page1 valid */
        }
      }
      else if (pagestatus0 == VALID_PAGE)
      {
        /* Page1 receiving data */
        if (pagestatus1 == RECEIVE_DATA)
        {
          return PAGE1;         /* Page1 valid */
        }
        else
        {
          return PAGE0;         /* Page0 valid */
        }
      }
      else
      {
        return NO_VALID_PAGE;   /* No valid Page */
      }

    case READ_FROM_VALID_PAGE:  /* ---- Read operation ---- */
      if (pagestatus0 == VALID_PAGE)
      {
        return PAGE0;           /* Page0 valid */
      }
      else if (pagestatus1 == VALID_PAGE)
      {
        return PAGE1;           /* Page1 valid */
      }
      else
      {
        return NO_VALID_PAGE ;  /* No valid Page */
      }

    default:
      return PAGE0;             /* Page0 valid */
  }
}

/**
  * @brief  Verify if active page is full and Writes variable in EEPROM.
  * @param  VirtAddress: 16 bit virtual address of the variable
  * @param  Data: 16 bit data to be written as variable value
  * @retval Success or error status:
  *           - FLASH_COMPLETE: on success
  *           - PAGE_FULL: if valid page is full
  *           - NO_VALID_PAGE: if no valid page was found
  *           - Flash error code: on write Flash error
  */
static uint16_t EE_VerifyPageFullWriteVariable(uint16_t VirtAddress, uint16_t Data)
{
  HAL_StatusTypeDef flashstatus = HAL_OK;
  uint16_t validpage = PAGE0;
  uint32_t address = EEPROM_START_ADDRESS, pageendaddress = EEPROM_START_ADDRESS+PAGE_SIZE;

  /* Get valid Page for write operation */
  validpage = EE_FindValidPage(WRITE_IN_VALID_PAGE);

  /* Check if there is no valid page */
  if (validpage == NO_VALID_PAGE)
  {
    return  NO_VALID_PAGE;
  }

  /* Get the valid Page start address */
  address = (uint32_t)(EEPROM_START_ADDRESS + (uint32_t)(validpage * PAGE_SIZE));

  /* Get the valid Page end address */
  pageendaddress = (uint32_t)((EEPROM_START_ADDRESS - 1) + (uint32_t)((validpage + 1) * PAGE_SIZE));

  /* Check each active page address starting from begining */
  while (address < pageendaddress)
  {
    /* Verify if address and address+2 contents are 0xFFFFFFFF */
    if ((*(__IO uint32_t*)address) == 0xFFFFFFFF)
    {
      /* Set variable data */
      flashstatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, Data);
      /* If program operation was failed, a Flash error code is returned */
      if (flashstatus != HAL_OK)
      {
        return flashstatus;
      }
      /* Set variable virtual address */
      flashstatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address + 2, VirtAddress);
      /* Return program operation status */
      return flashstatus;
    }
    else
    {
      /* Next address location */
      address = address + 4;
    }
  }

  /* Return PAGE_FULL in case the valid page is full */
  return PAGE_FULL;
}

/**
  * @brief  Transfers last updated variables data from the full Page to
  *   an empty one.
  * @param  VirtAddress: 16 bit virtual address of the variable
  * @param  Data: 16 bit data to be written as variable value
  * @retval Success or error status:
  *           - FLASH_COMPLETE: on success
  *           - PAGE_FULL: if valid page is full
  *           - NO_VALID_PAGE: if no valid page was found
  *           - Flash error code: on write Flash error
  */
static uint16_t EE_PageTransfer(uint16_t VirtAddress, uint16_t Data)
{
  HAL_StatusTypeDef flashstatus = HAL_OK;
  uint32_t newpageaddress = EEPROM_START_ADDRESS;
  uint32_t oldpageid = 0;
  uint16_t validpage = PAGE0, varidx = 0;
  uint16_t eepromstatus = 0, readstatus = 0;
  uint32_t page_error = 0;
  FLASH_EraseInitTypeDef s_eraseinit;

  /* Get active Page for read operation */
  validpage = EE_FindValidPage(READ_FROM_VALID_PAGE);

  if (validpage == PAGE1)       /* Page1 valid */
  {
    /* New page address where variable will be moved to */
    newpageaddress = PAGE0_BASE_ADDRESS;

    /* Old page ID where variable will be taken from */
    oldpageid = PAGE1_ID;
  }
  else if (validpage == PAGE0)  /* Page0 valid */
  {
    /* New page address  where variable will be moved to */
    newpageaddress = PAGE1_BASE_ADDRESS;

    /* Old page ID where variable will be taken from */
    oldpageid = PAGE0_ID;
  }
  else
  {
    return NO_VALID_PAGE;       /* No valid Page */
  }

  /* Set the new Page status to RECEIVE_DATA status */
  flashstatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, newpageaddress, RECEIVE_DATA);
  /* If program operation was failed, a Flash error code is returned */
  if (flashstatus != HAL_OK)
  {
    return flashstatus;
  }

  /* Write the variable passed as parameter in the new active page */
  eepromstatus = EE_VerifyPageFullWriteVariable(VirtAddress, Data);
  /* If program operation was failed, a Flash error code is returned */
  if (eepromstatus != HAL_OK)
  {
    return eepromstatus;
  }

  /* Transfer process: transfer variables from old to the new active page */
  for (varidx = 0; varidx < NB_OF_VAR; varidx++)
  {
    if (VirtAddVarTab[varidx] != VirtAddress)  /* Check each variable except the one passed as parameter */
    {
      /* Read the other last variable updates */
      readstatus = EE_ReadVariable(VirtAddVarTab[varidx], &DataVar);
      /* In case variable corresponding to the virtual address was found */
      if (readstatus != 0x1)
      {
        /* Transfer the variable to the new active page */
        eepromstatus = EE_VerifyPageFullWriteVariable(VirtAddVarTab[varidx], DataVar);
        /* If program operation was failed, a Flash error code is returned */
        if (eepromstatus != HAL_OK)
        {
          return eepromstatus;
        }
      }
    }
  }

  s_eraseinit.TypeErase   = FLASH_TYPEERASE_PAGES;
  s_eraseinit.PageAddress = oldpageid;
  s_eraseinit.NbPages     = 1;

  /* Erase the old Page: Set old Page status to ERASED status */
  flashstatus = HAL_FLASHEx_Erase(&s_eraseinit, &page_error);
  /* If erase operation was failed, a Flash error code is returned */
  if (flashstatus != HAL_OK)
  {
    return flashstatus;
  }

  /* Set new Page status to VALID_PAGE status */
  flashstatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, newpageaddress, VALID_PAGE);
  /* If program operation was failed, a Flash error code is returned */
  if (flashstatus != HAL_OK)
  {
    return flashstatus;
  }



  /* Return last operation flash status */
  return flashstatus;
}



