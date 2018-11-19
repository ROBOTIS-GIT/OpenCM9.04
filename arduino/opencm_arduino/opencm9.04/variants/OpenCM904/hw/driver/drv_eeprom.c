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
  0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,

  0x100, 0x101, 0x102, 0x103, 0x104, 0x105, 0x106, 0x107, 0x108, 0x109, 0x10a, 0x10b, 0x10c, 0x10d, 0x10e, 0x10f,
  0x110, 0x111, 0x112, 0x113, 0x114, 0x115, 0x116, 0x117, 0x118, 0x119, 0x11a, 0x11b, 0x11c, 0x11d, 0x11e, 0x11f,
  0x120, 0x121, 0x122, 0x123, 0x124, 0x125, 0x126, 0x127, 0x128, 0x129, 0x12a, 0x12b, 0x12c, 0x12d, 0x12e, 0x12f,
  0x130, 0x131, 0x132, 0x133, 0x134, 0x135, 0x136, 0x137, 0x138, 0x139, 0x13a, 0x13b, 0x13c, 0x13d, 0x13e, 0x13f,
  0x130, 0x141, 0x142, 0x143, 0x144, 0x145, 0x146, 0x147, 0x148, 0x149, 0x14a, 0x14b, 0x14c, 0x14d, 0x14e, 0x14f,
  0x150, 0x151, 0x152, 0x153, 0x154, 0x155, 0x156, 0x157, 0x158, 0x159, 0x15a, 0x15b, 0x15c, 0x15d, 0x15e, 0x15f,
  0x160, 0x161, 0x162, 0x163, 0x164, 0x165, 0x166, 0x167, 0x168, 0x169, 0x16a, 0x16b, 0x16c, 0x16d, 0x16e, 0x16f,
  0x170, 0x171, 0x172, 0x173, 0x174, 0x175, 0x176, 0x177, 0x178, 0x179, 0x17a, 0x17b, 0x17c, 0x17d, 0x17e, 0x17f,
  0x180, 0x181, 0x182, 0x183, 0x184, 0x185, 0x186, 0x187, 0x188, 0x189, 0x18a, 0x18b, 0x18c, 0x18d, 0x18e, 0x18f,
  0x190, 0x191, 0x192, 0x193, 0x194, 0x195, 0x196, 0x197, 0x198, 0x199, 0x19a, 0x19b, 0x19c, 0x19d, 0x19e, 0x19f,
  0x1a0, 0x1a1, 0x1a2, 0x1a3, 0x1a4, 0x1a5, 0x1a6, 0x1a7, 0x1a8, 0x1a9, 0x1aa, 0x1ab, 0x1ac, 0x1ad, 0x1ae, 0x1af,
  0x1b0, 0x1b1, 0x1b2, 0x1b3, 0x1b4, 0x1b5, 0x1b6, 0x1b7, 0x1b8, 0x1b9, 0x1ba, 0x1bb, 0x1bc, 0x1bd, 0x1be, 0x1bf,
  0x1c0, 0x1c1, 0x1c2, 0x1c3, 0x1c4, 0x1c5, 0x1c6, 0x1c7, 0x1c8, 0x1c9, 0x1ca, 0x1cb, 0x1cc, 0x1cd, 0x1ce, 0x1cf,
  0x1d0, 0x1d1, 0x1d2, 0x1d3, 0x1d4, 0x1d5, 0x1d6, 0x1d7, 0x1d8, 0x1d9, 0x1da, 0x1db, 0x1dc, 0x1dd, 0x1de, 0x1df,
  0x1e0, 0x1e1, 0x1e2, 0x1e3, 0x1e4, 0x1e5, 0x1e6, 0x1e7, 0x1e8, 0x1e9, 0x1ea, 0x1eb, 0x1ec, 0x1ed, 0x1ee, 0x1ef,
  0x1f0, 0x1f1, 0x1f2, 0x1f3, 0x1f4, 0x1f5, 0x1f6, 0x1f7, 0x1f8, 0x1f9, 0x1fa, 0x1fb, 0x1fc, 0x1fd, 0x1fe, 0x1ff
};

#else
#define VIRTADDVARTAB(index)   (index)
#endif

uint16_t EE_Init(void);
uint16_t EE_ReadVariable(uint16_t VirtAddress, uint16_t* Data);
uint16_t EE_WriteVariable(uint16_t VirtAddress, uint16_t Data);





int drv_eeprom_init()
{
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
  /* If program operation was failed, a Flash error code is returned */
  if (flashstatus != HAL_OK)
  {
    return flashstatus;
  }



  /* Return last operation flash status */
  return flashstatus;
}



