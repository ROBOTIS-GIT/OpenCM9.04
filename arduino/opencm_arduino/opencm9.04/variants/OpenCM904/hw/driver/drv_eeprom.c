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
#define NB_OF_VAR             (255)

//#define USE_VIRT_ADD_VAR_TAB
#ifdef USE_VIRT_ADD_VAR_TAB
#define VIRTADDVARTAB(index)   (VirtAddVarTab[index])

/* Virtual address defined by the user: 0xFFFF value is prohibited */
static const uint16_t VirtAddVarTab[NB_OF_VAR] = 
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
  0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
  0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
  0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
  0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
  0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
  0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
  0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe
};

#else
#define VIRTADDVARTAB(index)   (index)
#endif

uint16_t EE_Init(void);
uint16_t EE_ReadVariable(uint16_t VirtAddress, uint16_t* Data);
uint16_t EE_WriteVariable(uint16_t VirtAddress, uint16_t Data);
static uint16_t EE_PageConvert(uint16_t validpage);



// Lets try defer of EEPROM code until someone needs it
bool drv_eeprom_is_valid(void)
{
  if (!IsInit) 
  {
    if( EE_Init() == HAL_OK )
    {
      IsInit = true;
    }    
  }
  return IsInit;
}


int drv_eeprom_init()
{
  HAL_FLASH_Unlock();
#if 0  // we will try to defer the actual init until we need it
  if( EE_Init() == HAL_OK )
  {
    IsInit = true;
  }
#endif  
  return 0;
}



uint8_t drv_eeprom_read_byte(int addr)
{
  uint16_t read_value;


  if( !drv_eeprom_is_valid()) return 0;

  EE_ReadVariable((uint16_t)addr,  &read_value);

  return (uint8_t)read_value;
}


void drv_eeprom_write_byte(int index, uint8_t data_in)
{
  if( !drv_eeprom_is_valid()) return;

  EE_WriteVariable(index, (uint16_t)data_in);
}


uint16_t drv_eeprom_get_length(void)
{
  if( !drv_eeprom_is_valid()) return 0;

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
#define PAGE1                 ((uint16_t)0x0001)

/* No valid page define */
#define NO_VALID_PAGE         ((uint16_t)0x00AB)

/* Page status definitions */
#define ERASED                ((uint16_t)0xFFFF)     /* Page is empty */
#define RECEIVE_DATA          ((uint16_t)0xEEEE)     /* Page is marked to receive data */
#define VALID_PAGE            ((uint16_t)0x0000)     /* Page containing valid data */
#define PAGE_VERSION          ((uint16_t)0x0002)     /* Version 2 - Version 1 was 0xffff */

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
        // Check and maybe convert page format
        flashstatus = EE_PageConvert(PAGE1);
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
        flashstatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, PAGE1_BASE_ADDRESS+2, PAGE_VERSION);
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
          if (( *(__IO uint16_t*)(PAGE0_BASE_ADDRESS + 6)) == VIRTADDVARTAB(varidx))
          {
            x = varidx;
          }
          if (varidx != x)
          {
            /* Read the last variables' updates */
            readstatus = EE_ReadVariable(VIRTADDVARTAB(varidx), &DataVar);
            /* In case variable corresponding to the virtual address was found */
            if (readstatus != 0x1)
            {
              /* Transfer the variable to the Page0 */
              eepromstatus = EE_VerifyPageFullWriteVariable(VIRTADDVARTAB(varidx), DataVar);
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
        flashstatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, PAGE0_BASE_ADDRESS+2, PAGE_VERSION);
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
        flashstatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, PAGE0_BASE_ADDRESS+2, PAGE_VERSION);
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
        // Check and maybe convert page format
        flashstatus = EE_PageConvert(PAGE0);
      }
      else /* Page0 valid, Page1 receive */
      {
        /* Transfer data from Page0 to Page1 */
        for (varidx = 0; varidx < NB_OF_VAR; varidx++)
        {
          if ((*(__IO uint16_t*)(PAGE1_BASE_ADDRESS + 6)) == VIRTADDVARTAB(varidx))
          {
            x = varidx;
          }
          if (varidx != x)
          {
            /* Read the last variables' updates */
            readstatus = EE_ReadVariable(VIRTADDVARTAB(varidx), &DataVar);
            /* In case variable corresponding to the virtual address was found */
            if (readstatus != 0x1)
            {
              /* Transfer the variable to the Page1 */
              eepromstatus = EE_VerifyPageFullWriteVariable(VIRTADDVARTAB(varidx), DataVar);
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
        flashstatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, PAGE1_BASE_ADDRESS+2, PAGE_VERSION);
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

  uint32_t page_end_address = Address + PAGE_SIZE;
  /* Check each active page address starting from end */
  while (Address < page_end_address)
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
    Address = Address + 2;
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
  address = PageStartAddress + PAGE_SIZE - 2;
  PageStartAddress += 2;    // Also don't look at the page verison number we stored here.
  /* Check each active page address starting from end */
  while (address > PageStartAddress)
  {
    /* Get the current location content to be compared with virtual address */
    addressvalue = (*(__IO uint16_t*)address);

    /* Compare the read address with the virtual address */
    /* With this 2 byte format Low byte is address, high byte is value */
    if ((addressvalue & 0xff) == VirtAddress)
    {
      /* Get content of Address-2 which is variable value */
      *Data = (addressvalue >> 8) & 0xff;

      /* In case variable value is read, reset readstatus flag */
      readstatus = 0;

      break;
    }
    else
    {
      /* Next address location */
      address = address - 2;
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

  if (VirtAddress >= NB_OF_VAR)
  {
    return Status;  // Bail if address exceeds are max... 
  }

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
  uint16_t addressvalue = 0x5555;
  uint16_t cur_Data = !Data;  // initialize to make sure it does not match...

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
  pageendaddress = address + PAGE_SIZE;
  address += 4; // don't try using the Page type and Page version fields

  /* Check each active page address starting from begining */
  while (address < pageendaddress)
  {
    /* Verify if address contents are 0xFFFF */
    addressvalue = (*(__IO uint16_t*)address);
    if (addressvalue == 0xFFFF)
    {
      /* Found an empty slot, see if our index is in the list and the last */
      /* Instance has the value we are trying to store, we can bypass this. */
      if (cur_Data == Data)
      {
        return HAL_OK;  // Value is already set to our value
      }
      /* Set variable and address */
      flashstatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, (Data << 8) + VirtAddress);;
      /* Return program operation status */
      return flashstatus;
    }
    else
    {
      /* Compare the read address with the virtual address */
      if ((addressvalue & 0xff) == VirtAddress)
      {
        /* Get content of Address-2 which is variable value */
        cur_Data = (addressvalue >> 8) & 0xff;

      }
      /* Next address location */
      address = address + 2;
    }
  }

  /* We did not find any open slots, so if we did not find oru value */
  /* Return PAGE_FULL in case the valid page is full */
  return (cur_Data == Data)? HAL_OK : PAGE_FULL;
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
    if (VIRTADDVARTAB(varidx) != VirtAddress)  /* Check each variable except the one passed as parameter */
    {
      /* Read the other last variable updates */
      readstatus = EE_ReadVariable(VIRTADDVARTAB(varidx), &DataVar);
      /* In case variable corresponding to the virtual address was found */
      if (readstatus != 0x1)
      {
        /* Transfer the variable to the new active page */
        eepromstatus = EE_VerifyPageFullWriteVariable(VIRTADDVARTAB(varidx), DataVar);
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
  flashstatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, newpageaddress+2, PAGE_VERSION);

  /* Return last operation flash status */
  return flashstatus;
}


/**
  * @brief  Convert page from 4 bytes per entry to 2...
  * @param  validPage: 16 bit virtual address of the variable
  * @retval Success or error status:
  *           - FLASH_COMPLETE: on success
  *           - PAGE_FULL: if valid page is full
  *           - NO_VALID_PAGE: if no valid page was found
  *           - Flash error code: on write Flash error
  */
static uint16_t EE_PageConvert(uint16_t validpage)
{
  HAL_StatusTypeDef flashstatus = HAL_OK;
  uint32_t newpageaddress = EEPROM_START_ADDRESS;
  uint32_t oldpageaddress;
  uint32_t oldpageaddress_end;
  uint32_t oldpageid = 0;
  uint16_t varidx = 0;
  uint16_t eepromstatus = 0, readstatus = 0;
  uint32_t page_error = 0;
  FLASH_EraseInitTypeDef s_eraseinit;

  // vcp_printf("EE_PageConvert: %\n", validpage);
  s_eraseinit.TypeErase   = FLASH_TYPEERASE_PAGES;
  s_eraseinit.NbPages     = 1;

  if (validpage == PAGE1)       /* Page1 valid */
  {
    /* New page address where variable will be moved to */
    newpageaddress = PAGE0_BASE_ADDRESS;
    oldpageaddress = PAGE1_BASE_ADDRESS;
    s_eraseinit.PageAddress = PAGE0_ID;

    /* Old page ID where variable will be taken from */
    oldpageid = PAGE1_ID;
  }
  else if (validpage == PAGE0)  /* Page0 valid */
  {
    /* New page address  where variable will be moved to */
    newpageaddress = PAGE1_BASE_ADDRESS;
    oldpageaddress = PAGE0_BASE_ADDRESS;

    s_eraseinit.PageAddress = PAGE1_ID;
    /* Old page ID where variable will be taken from */
    oldpageid = PAGE0_ID;
  }

  // Lets see if we need to co a conversion.
  if (((*(__IO uint16_t*)oldpageaddress) == VALID_PAGE) && ((*(__IO uint16_t*)(oldpageaddress+2)) == PAGE_VERSION))
  {
    // vcp_printf("Page is valid and new version\n");
    return HAL_OK;

  }

  // Make sure the new page is erased. 
  if(!EE_VerifyPageFullyErased(newpageaddress))
  {
    // vcp_printf(" Need to erase new page\n");
    flashstatus = HAL_FLASHEx_Erase(&s_eraseinit, &page_error);
    /* If erase operation was failed, a Flash error code is returned */
    if (flashstatus != HAL_OK)
    {
      return flashstatus;
    }
  }

  /* Set the new Page status to RECEIVE_DATA status */
  flashstatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, newpageaddress, RECEIVE_DATA);
  /* If program operation was failed, a Flash error code is returned */
  if (flashstatus != HAL_OK)
  {
    return flashstatus;
  }


  /* Transfer process: transfer variables from old to the new active page */
  /* First find the end of the old data */
  oldpageaddress_end = oldpageaddress + PAGE_SIZE - 4;
  while ( (*(__IO uint32_t*)oldpageaddress_end) == 0xFFFFFFFF)
  {
    oldpageaddress_end -= 4;
  }

  // vcp_printf("Old Page start:%x end:%x\n", (uint32_t)oldpageaddress, (uint32_t)oldpageaddress_end);

  for (varidx = 0; varidx < NB_OF_VAR; varidx++)
  {
    /* See if we find that data in the old data */
    for (uint32_t address = oldpageaddress_end; address > oldpageaddress; address -= 4)
    {
      if ((*(__IO uint16_t*)(address+2)) == varidx)
      {
        DataVar = (*(__IO uint16_t*)address) & 0xff;
        // vcp_printf("transfer %x:%x\n", varidx, DataVar);
        eepromstatus = EE_VerifyPageFullWriteVariable(VIRTADDVARTAB(varidx), DataVar);
        /* If program operation was failed, a Flash error code is returned */
        if (eepromstatus != HAL_OK)
        {
          // vcp_printf("Write failed(%x) %x:%x\n", eepromstatus, varidx, DataVar);
          return eepromstatus;
        }
        break;
      }
    }
  }

  s_eraseinit.TypeErase   = FLASH_TYPEERASE_PAGES;
  s_eraseinit.PageAddress = oldpageid;
  s_eraseinit.NbPages     = 1;
  // vcp_printf("Erase old page %d\n", oldpageid);
  /* Erase the old Page: Set old Page status to ERASED status */
  flashstatus = HAL_FLASHEx_Erase(&s_eraseinit, &page_error);
  /* If erase operation was failed, a Flash error code is returned */
  if (flashstatus != HAL_OK)
  {
    return flashstatus;
  }

  /* Set new Page status to VALID_PAGE status */
  // vcp_printf("Set newpage to valid\n");
  flashstatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, newpageaddress, VALID_PAGE);
  flashstatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, newpageaddress+2, PAGE_VERSION);

  /* Return last operation flash status */
  return flashstatus;
}
