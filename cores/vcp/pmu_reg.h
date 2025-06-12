// SPDX-License-Identifier: Apache-2.0

/*
***************************************************************************************************
*
*   FileName : pmu_reg.h
*
*   Copyright (c) Telechips Inc.
*
*   Description :
*
*
***************************************************************************************************
*/
#ifndef MCU_BSP_PMU_REG_HEADER
#define MCU_BSP_PMU_REG_HEADER


/*
***************************************************************************************************
*                                             INCLUDE FILES
***************************************************************************************************
*/
#include <sal_com.h>


/*
***************************************************************************************************
*                                             DEFINITIONS
***************************************************************************************************
*/

/* Common (Register function) ===============================================*/
#define PMU_REG_GET(o, m, a)           ( ((*(SALReg32 *)((uint32_t)a)) & (uint32_t)m ) >> (uint32_t)o )
#define PMU_REG_SET(x, o, m, a)        (*(SALReg32 *)((uint32_t)a)) = (((uint32_t)x << (uint32_t)o) & (uint32_t)m)
#define PMU_REG_APPEND(x, o, m, a)     ( (*(SALReg32 *)((uint32_t)a)) = ( ((*(SALReg32 *)((uint32_t)a)) & (~((uint32_t) m))) | ((uint32_t)x << o) ) )

/* Common (Bit Flags) ===============================================*/
#define PMU_VA_MASK_1                  (0x1UL)
#define PMU_VA_MASK_2                  (0x3UL)
#define PMU_VA_MASK_3                  (0x7UL)
#define PMU_VA_MASK_4                  (0xFUL)
#define PMU_VA_MASK_5                  (0x1FUL)
#define PMU_VA_MASK_6                  (0x3FUL)
#define PMU_VA_MASK_7                  (0x7FUL)
#define PMU_VA_MASK_8                  (0xFFUL)
#define PMU_VA_MASK_9                  (0x1FFUL)
#define PMU_VA_MASK_10                 (0x3FFUL)
#define PMU_VA_MASK_11                 (0x7FFUL)
#define PMU_VA_MASK_12                 (0xFFFUL)
#define PMU_VA_MASK_13                 (0x1FFFUL)
#define PMU_VA_MASK_14                 (0x3FFFUL)
#define PMU_VA_MASK_15                 (0x7FFFUL)
#define PMU_VA_MASK_16                 (0xFFFFUL)
#define PMU_VA_MASK_17                 (0x1FFFFUL)
#define PMU_VA_MASK_18                 (0x3FFFFUL)
#define PMU_VA_MASK_19                 (0x7FFFFUL)
#define PMU_VA_MASK_20                 (0xFFFFFUL)
#define PMU_VA_MASK_21                 (0x1FFFFFUL)
#define PMU_VA_MASK_22                 (0x3FFFFFUL)
#define PMU_VA_MASK_23                 (0x7FFFFFUL)
#define PMU_VA_MASK_24                 (0xFFFFFFUL)
#define PMU_VA_MASK_25                 (0x1FFFFFFUL)
#define PMU_VA_MASK_26                 (0x3FFFFFFUL)
#define PMU_VA_MASK_27                 (0x7FFFFFFUL)
#define PMU_VA_MASK_28                 (0xFFFFFFFUL)
#define PMU_VA_MASK_29                 (0x1FFFFFFFUL)
#define PMU_VA_MASK_30                 (0x3FFFFFFFUL)
#define PMU_VA_MASK_31                 (0x7FFFFFFFUL)
#define PMU_VA_MASK_32                 (0xFFFFFFFFUL)

/* REGISTER INFO (Map: PMU Address table) ==================================*/
#define PMU_ADDR_BASE                                                 (0xA0F28000)
#define PMU_ADDR_GLB_CONFIG                                           (PMU_ADDR_BASE + 0x00UL)
#define PMU_ADDR_RST_STATUS0                                          (PMU_ADDR_BASE + 0x04UL)
#define PMU_ADDR_RST_STATUS1                                          (PMU_ADDR_BASE + 0x08UL)
#define PMU_ADDR_RST_ENABLE                                           (PMU_ADDR_BASE + 0x0CUL)
#define PMU_ADDR_COLD_RST_REQ                                         (PMU_ADDR_BASE + 0x10UL)
#define PMU_ADDR_WARM_RST_REQ                                         (PMU_ADDR_BASE + 0x14UL)
#define PMU_ADDR_HSM_RSTN_MSK                                         (PMU_ADDR_BASE + 0x18UL)
#define PMU_ADDR_HSM_SWRSTN                                           (PMU_ADDR_BASE + 0x1CUL)
#define PMU_ADDR_HSM_STATUS                                           (PMU_ADDR_BASE + 0x20UL)
#define PMU_ADDR_SYSRST_CTRL                                          (PMU_ADDR_BASE + 0x24UL)
#define PMU_ADDR_MEM_ECC_CFG                                          (PMU_ADDR_BASE + 0x28UL)
#define PMU_ADDR_PVT0_CFG                                             (PMU_ADDR_BASE + 0x2CUL)
#define PMU_ADDR_PVT1_CFG                                             (PMU_ADDR_BASE + 0x30UL)
#define PMU_ADDR_PVT2_CFG                                             (PMU_ADDR_BASE + 0x34UL)
#define PMU_ADDR_PVT3_CFG                                             (PMU_ADDR_BASE + 0x38UL)
#define PMU_ADDR_PVT4_CFG                                             (PMU_ADDR_BASE + 0x3CUL)
#define PMU_ADDR_PVT_SM_CFG                                           (PMU_ADDR_BASE + 0x40UL)
#define PMU_ADDR_XIN_SELECT_FREQ                                      (PMU_ADDR_BASE + 0x44UL)
#define PMU_ADDR_OSC_CFG                                              (PMU_ADDR_BASE + 0x48UL)
#define PMU_ADDR_ECID_USER0_FBOUT0                                    (PMU_ADDR_BASE + 0x4CUL)
#define PMU_ADDR_ECID_USER0_FBOUT1                                    (PMU_ADDR_BASE + 0x50UL)
#define PMU_ADDR_ECID_USER0_FBOUT2                                    (PMU_ADDR_BASE + 0x54UL)
#define PMU_ADDR_ECID_USER1_FBOUT0                                    (PMU_ADDR_BASE + 0x58UL)
#define PMU_ADDR_ECID_USER1_FBOUT1                                    (PMU_ADDR_BASE + 0x5CUL)
#define PMU_ADDR_ECID_USER1_FBOUT2                                    (PMU_ADDR_BASE + 0x60UL)
#define PMU_ADDR_MEM_CFG                                              (PMU_ADDR_BASE + 0x64UL)
#define PMU_ADDR_BACKUP_REG                                           (PMU_ADDR_BASE + 0x68UL)
#define PMU_ADDR_PMU_WDT_EN                                           (PMU_ADDR_BASE + 0x6CUL)
#define PMU_ADDR_PMU_WDT_CLR                                          (PMU_ADDR_BASE + 0x70UL)
#define PMU_ADDR_PMU_WDT_IRQ_CNT                                      (PMU_ADDR_BASE + 0x74UL)
#define PMU_ADDR_PMU_WDT_RST_CNT                                      (PMU_ADDR_BASE + 0x78UL)
#define PMU_ADDR_PMU_WDT_SM_MODE                                      (PMU_ADDR_BASE + 0x7CUL)
#define PMU_ADDR_PMU_WDT_LOCK                                         (PMU_ADDR_BASE + 0x80UL)
#define PMU_ADDR_PMU_WR_PW                                            (PMU_ADDR_BASE + 0x3FCUL)

/* REGISTER INFO (Map: PMU Register controller) ============================*/
#define PMU_REG_GLB_CONFIG                                            (*(SALReg32 *)(PMU_ADDR_GLB_CONFIG))
#define PMU_REG_RST_STATUS0                                           (*(SALReg32 *)(PMU_ADDR_RST_STATUS0))
#define PMU_REG_RST_STATUS1                                           (*(SALReg32 *)(PMU_ADDR_RST_STATUS1))
#define PMU_REG_RST_ENABLE                                            (*(SALReg32 *)(PMU_ADDR_RST_ENABLE))
#define PMU_REG_COLD_RST_REQ                                          (*(SALReg32 *)(PMU_ADDR_COLD_RST_REQ))
#define PMU_REG_WARM_RST_REQ                                          (*(SALReg32 *)(PMU_ADDR_WARM_RST_REQ))
#define PMU_REG_HSM_RSTN_MSK                                          (*(SALReg32 *)(PMU_ADDR_HSM_RSTN_MSK))
#define PMU_REG_HSM_SWRSTN                                            (*(SALReg32 *)(PMU_ADDR_HSM_SWRSTN))
#define PMU_REG_HSM_STATUS                                            (*(SALReg32 *)(PMU_ADDR_HSM_STATUS))
#define PMU_REG_SYSRST_CTRL                                           (*(SALReg32 *)(PMU_ADDR_SYSRST_CTRL))
#define PMU_REG_MEM_ECC_CFG                                           (*(SALReg32 *)(PMU_ADDR_MEM_ECC_CFG))
#define PMU_REG_PVT0_CFG                                              (*(SALReg32 *)(PMU_ADDR_PVT0_CFG))
#define PMU_REG_PVT1_CFG                                              (*(SALReg32 *)(PMU_ADDR_PVT1_CFG))
#define PMU_REG_PVT2_CFG                                              (*(SALReg32 *)(PMU_ADDR_PVT2_CFG))
#define PMU_REG_PVT3_CFG                                              (*(SALReg32 *)(PMU_ADDR_PVT3_CFG))
#define PMU_REG_PVT4_CFG                                              (*(SALReg32 *)(PMU_ADDR_PVT4_CFG))
#define PMU_REG_PVT_SM_CFG                                            (*(SALReg32 *)(PMU_ADDR_PVT_SM_CFG))
#define PMU_REG_XIN_SELECT_FREQ                                       (*(SALReg32 *)(PMU_ADDR_XIN_SELECT_FREQ))
#define PMU_REG_OSC_CFG                                               (*(SALReg32 *)(PMU_ADDR_OSC_CFG))
#define PMU_REG_ECID_USER0_FBOUT0                                     (*(SALReg32 *)(PMU_ADDR_ECID_USER0_FBOUT0))
#define PMU_REG_ECID_USER0_FBOUT1                                     (*(SALReg32 *)(PMU_ADDR_ECID_USER0_FBOUT1))
#define PMU_REG_ECID_USER0_FBOUT2                                     (*(SALReg32 *)(PMU_ADDR_ECID_USER0_FBOUT2))
#define PMU_REG_ECID_USER1_FBOUT0                                     (*(SALReg32 *)(PMU_ADDR_ECID_USER1_FBOUT0))
#define PMU_REG_ECID_USER1_FBOUT1                                     (*(SALReg32 *)(PMU_ADDR_ECID_USER1_FBOUT1))
#define PMU_REG_ECID_USER1_FBOUT2                                     (*(SALReg32 *)(PMU_ADDR_ECID_USER1_FBOUT2))
#define PMU_REG_MEM_CFG                                               (*(SALReg32 *)(PMU_ADDR_MEM_CFG))
#define PMU_REG_BACKUP_REG                                            (*(SALReg32 *)(PMU_ADDR_BACKUP_REG))
#define PMU_REG_PMU_WDT_EN                                            (*(SALReg32 *)(PMU_ADDR_PMU_WDT_EN))
#define PMU_REG_PMU_WDT_CLR                                           (*(SALReg32 *)(PMU_ADDR_PMU_WDT_CLR))
#define PMU_REG_PMU_WDT_IRQ_CNT                                       (*(SALReg32 *)(PMU_ADDR_PMU_WDT_IRQ_CNT))
#define PMU_REG_PMU_WDT_RST_CNT                                       (*(SALReg32 *)(PMU_ADDR_PMU_WDT_RST_CNT))
#define PMU_REG_PMU_WDT_SM_MODE                                       (*(SALReg32 *)(PMU_ADDR_PMU_WDT_SM_MODE))
#define PMU_REG_PMU_WDT_LOCK                                          (*(SALReg32 *)(PMU_ADDR_PMU_WDT_LOCK))
#define PMU_REG_PMU_WR_PW                                             (*(SALReg32 *)(PMU_ADDR_PMU_WR_PW))

/* FIELD INFO (Common) ===============================================*/
#define PMU_ADDR_COMMON_FIELD_ZERO                                    (0UL)
#define PMU_ADDR_COMMON_FIELD_FULL_MASK                               (PMU_VA_MASK_31)
#define PMU_ADDR_COMMON_FIELD_CLEAR_MASK                              (0UL)

/* FIELD INFO (Register: GLB_CONFIG) ===========================================*/
#define PMU_ADDR_GLB_CONFIG_FIELD_REMAP                               (16UL)
#define PMU_ADDR_GLB_CONFIG_FIELD_REMAP_MASK                          (PMU_VA_MASK_2 << PMU_ADDR_GLB_CONFIG_FIELD_REMAP)
#define PMU_ADDR_GLB_CONFIG_FIELD_BM                                  (0UL)
#define PMU_ADDR_GLB_CONFIG_FIELD_BM_MASK                             (PMU_VA_MASK_1 << PMU_ADDR_GLB_CONFIG_FIELD_BM)

/* FIELD INFO (Register: RST_STATUS0) ===========================================*/
#define PMU_ADDR_RST_STATUS0_FIELD_HSM_WDT_RST_STS                    (16UL)
#define PMU_ADDR_RST_STATUS0_FIELD_HSM_WDT_RST_STS_MASK               (PMU_VA_MASK_2 << PMU_ADDR_RST_STATUS0_FIELD_HSM_WDT_RST_STS)
#define PMU_ADDR_RST_STATUS0_FIELD_PMU_WDT_RST_STS                    (14UL)
#define PMU_ADDR_RST_STATUS0_FIELD_PMU_WDT_RST_STS_MASK               (PMU_VA_MASK_2 << PMU_ADDR_RST_STATUS0_FIELD_PMU_WDT_RST_STS)
#define PMU_ADDR_RST_STATUS0_FIELD_WDT_RST_STS                        (12UL)
#define PMU_ADDR_RST_STATUS0_FIELD_WDT_RST_STS_MASK                   (PMU_VA_MASK_2 << PMU_ADDR_RST_STATUS0_FIELD_WDT_RST_STS)
#define PMU_ADDR_RST_STATUS0_FIELD_HSM_WRAM_RST_STS                   (10UL)
#define PMU_ADDR_RST_STATUS0_FIELD_HSM_WRAM_RST_STS_MASK              (PMU_VA_MASK_2 << PMU_ADDR_RST_STATUS0_FIELD_HSM_WRAM_RST_STS)
#define PMU_ADDR_RST_STATUS0_FIELD_WARM_RST_STS                       (8UL)
#define PMU_ADDR_RST_STATUS0_FIELD_WARM_RST_STS_MASK                  (PMU_VA_MASK_2 << PMU_ADDR_RST_STATUS0_FIELD_WARM_RST_STS)
#define PMU_ADDR_RST_STATUS0_FIELD_FMU_FAULT_RST_STS                  (6UL)
#define PMU_ADDR_RST_STATUS0_FIELD_FMU_FAULT_RST_STS_MASK             (PMU_VA_MASK_2 << PMU_ADDR_RST_STATUS0_FIELD_FMU_FAULT_RST_STS)
#define PMU_ADDR_RST_STATUS0_FIELD_HSM_COLD_RST_STS                   (4UL)
#define PMU_ADDR_RST_STATUS0_FIELD_HSM_COLD_RST_STS_MASK              (PMU_VA_MASK_2 << PMU_ADDR_RST_STATUS0_FIELD_HSM_COLD_RST_STS)
#define PMU_ADDR_RST_STATUS0_FIELD_COLD_RST_STS                       (2UL)
#define PMU_ADDR_RST_STATUS0_FIELD_COLD_RST_STS_MASK                  (PMU_VA_MASK_2 << PMU_ADDR_RST_STATUS0_FIELD_COLD_RST_STS)
#define PMU_ADDR_RST_STATUS0_FIELD_PORn_STS                           (0UL)
#define PMU_ADDR_RST_STATUS0_FIELD_PORn_STS_MASK                      (PMU_VA_MASK_2 << PMU_ADDR_RST_STATUS0_FIELD_PORn_STS)

/* FIELD INFO (Register: RST_STATUS1) ===========================================*/
#define PMU_ADDR_RST_STATUS1_FIELD_PWR12_PVT_RST_STS                  (26UL)
#define PMU_ADDR_RST_STATUS1_FIELD_PWR12_PVT_RST_STS_MASK             (PMU_VA_MASK_2 << PMU_ADDR_RST_STATUS1_FIELD_PWR12_PVT_RST_STS)
#define PMU_ADDR_RST_STATUS1_FIELD_PWR11_PVT_RST_STS                  (24UL)
#define PMU_ADDR_RST_STATUS1_FIELD_PWR11_PVT_RST_STS_MASK             (PMU_VA_MASK_2 << PMU_ADDR_RST_STATUS1_FIELD_PWR11_PVT_RST_STS)
#define PMU_ADDR_RST_STATUS1_FIELD_PWR10_PVT_RST_STS                  (22UL)
#define PMU_ADDR_RST_STATUS1_FIELD_PWR10_PVT_RST_STS_MASK             (PMU_VA_MASK_2 << PMU_ADDR_RST_STATUS1_FIELD_PWR10_PVT_RST_STS)
#define PMU_ADDR_RST_STATUS1_FIELD_PWR9_PVT_RST_STS                   (20UL)
#define PMU_ADDR_RST_STATUS1_FIELD_PWR9_PVT_RST_STS_MASK              (PMU_VA_MASK_2 << PMU_ADDR_RST_STATUS1_FIELD_PWR9_PVT_RST_STS)
#define PMU_ADDR_RST_STATUS1_FIELD_PWR8_PVT_RST_STS                   (18UL)
#define PMU_ADDR_RST_STATUS1_FIELD_PWR8_PVT_RST_STS_MASK              (PMU_VA_MASK_2 << PMU_ADDR_RST_STATUS1_FIELD_PWR8_PVT_RST_STS)
#define PMU_ADDR_RST_STATUS1_FIELD_PWR7_PVT_RST_STS                   (16UL)
#define PMU_ADDR_RST_STATUS1_FIELD_PWR7_PVT_RST_STS_MASK              (PMU_VA_MASK_2 << PMU_ADDR_RST_STATUS1_FIELD_PWR7_PVT_RST_STS)
#define PMU_ADDR_RST_STATUS1_FIELD_PWR6_PVT_RST_STS                   (14UL)
#define PMU_ADDR_RST_STATUS1_FIELD_PWR6_PVT_RST_STS_MASK              (PMU_VA_MASK_2 << PMU_ADDR_RST_STATUS1_FIELD_PWR6_PVT_RST_STS)
#define PMU_ADDR_RST_STATUS1_FIELD_PWR5_PVT_RST_STS                   (12UL)
#define PMU_ADDR_RST_STATUS1_FIELD_PWR5_PVT_RST_STS_MASK              (PMU_VA_MASK_2 << PMU_ADDR_RST_STATUS1_FIELD_PWR5_PVT_RST_STS)
#define PMU_ADDR_RST_STATUS1_FIELD_PWR4_PVT_RST_STS                   (10UL)
#define PMU_ADDR_RST_STATUS1_FIELD_PWR4_PVT_RST_STS_MASK              (PMU_VA_MASK_2 << PMU_ADDR_RST_STATUS1_FIELD_PWR4_PVT_RST_STS)
#define PMU_ADDR_RST_STATUS1_FIELD_PWR3_PVT_RST_STS                   (8UL)
#define PMU_ADDR_RST_STATUS1_FIELD_PWR3_PVT_RST_STS_MASK              (PMU_VA_MASK_2 << PMU_ADDR_RST_STATUS1_FIELD_PWR3_PVT_RST_STS)
#define PMU_ADDR_RST_STATUS1_FIELD_PWR2_PVT_RST_STS                   (6UL)
#define PMU_ADDR_RST_STATUS1_FIELD_PWR2_PVT_RST_STS_MASK              (PMU_VA_MASK_2 << PMU_ADDR_RST_STATUS1_FIELD_PWR2_PVT_RST_STS)
#define PMU_ADDR_RST_STATUS1_FIELD_PWR1_PVT_RST_STS                   (4UL)
#define PMU_ADDR_RST_STATUS1_FIELD_PWR1_PVT_RST_STS_MASK              (PMU_VA_MASK_2 << PMU_ADDR_RST_STATUS1_FIELD_PWR1_PVT_RST_STS)
#define PMU_ADDR_RST_STATUS1_FIELD_PWR0_PVT_RST_STS                   (2UL)
#define PMU_ADDR_RST_STATUS1_FIELD_PWR0_PVT_RST_STS_MASK              (PMU_VA_MASK_2 << PMU_ADDR_RST_STATUS1_FIELD_PWR0_PVT_RST_STS)
#define PMU_ADDR_RST_STATUS1_FIELD_XIN_PVT_RST_STS                    (0UL)
#define PMU_ADDR_RST_STATUS1_FIELD_XIN_PVT_RST_STS_MASK               (PMU_VA_MASK_2 << PMU_ADDR_RST_STATUS1_FIELD_XIN_PVT_RST_STS)

/* FIELD INFO (Register: RST_ENABLE) ===========================================*/
#define PMU_ADDR_RST_ENABLE_FIELD_PWR12_PVT_RST_EN                    (19UL)
#define PMU_ADDR_RST_ENABLE_FIELD_PWR12_PVT_RST_EN_MASK               (PMU_VA_MASK_1 << PMU_ADDR_RST_ENABLE_FIELD_PWR12_PVT_RST_EN)
#define PMU_ADDR_RST_ENABLE_FIELD_PWR11_PVT_RST_EN                    (18UL)
#define PMU_ADDR_RST_ENABLE_FIELD_PWR11_PVT_RST_EN_MASK               (PMU_VA_MASK_1 << PMU_ADDR_RST_ENABLE_FIELD_PWR11_PVT_RST_EN)
#define PMU_ADDR_RST_ENABLE_FIELD_PWR10_PVT_RST_EN                    (17UL)
#define PMU_ADDR_RST_ENABLE_FIELD_PWR10_PVT_RST_EN_MASK               (PMU_VA_MASK_1 << PMU_ADDR_RST_ENABLE_FIELD_PWR10_PVT_RST_EN)
#define PMU_ADDR_RST_ENABLE_FIELD_PWR9_PVT_RST_EN                     (16UL)
#define PMU_ADDR_RST_ENABLE_FIELD_PWR9_PVT_RST_EN_MASK                (PMU_VA_MASK_1 << PMU_ADDR_RST_ENABLE_FIELD_PWR9_PVT_RST_EN)
#define PMU_ADDR_RST_ENABLE_FIELD_PWR8_PVT_RST_EN                     (15UL)
#define PMU_ADDR_RST_ENABLE_FIELD_PWR8_PVT_RST_EN_MASK                (PMU_VA_MASK_1 << PMU_ADDR_RST_ENABLE_FIELD_PWR8_PVT_RST_EN)
#define PMU_ADDR_RST_ENABLE_FIELD_PWR7_PVT_RST_EN                     (14UL)
#define PMU_ADDR_RST_ENABLE_FIELD_PWR7_PVT_RST_EN_MASK                (PMU_VA_MASK_1 << PMU_ADDR_RST_ENABLE_FIELD_PWR7_PVT_RST_EN)
#define PMU_ADDR_RST_ENABLE_FIELD_PWR6_PVT_RST_EN                     (13UL)
#define PMU_ADDR_RST_ENABLE_FIELD_PWR6_PVT_RST_EN_MASK                (PMU_VA_MASK_1 << PMU_ADDR_RST_ENABLE_FIELD_PWR6_PVT_RST_EN)
#define PMU_ADDR_RST_ENABLE_FIELD_PWR5_PVT_RST_EN                     (12UL)
#define PMU_ADDR_RST_ENABLE_FIELD_PWR5_PVT_RST_EN_MASK                (PMU_VA_MASK_1 << PMU_ADDR_RST_ENABLE_FIELD_PWR5_PVT_RST_EN)
#define PMU_ADDR_RST_ENABLE_FIELD_PWR4_PVT_RST_EN                     (11UL)
#define PMU_ADDR_RST_ENABLE_FIELD_PWR4_PVT_RST_EN_MASK                (PMU_VA_MASK_1 << PMU_ADDR_RST_ENABLE_FIELD_PWR4_PVT_RST_EN)
#define PMU_ADDR_RST_ENABLE_FIELD_PWR3_PVT_RST_EN                     (10UL)
#define PMU_ADDR_RST_ENABLE_FIELD_PWR3_PVT_RST_EN_MASK                (PMU_VA_MASK_1 << PMU_ADDR_RST_ENABLE_FIELD_PWR3_PVT_RST_EN)
#define PMU_ADDR_RST_ENABLE_FIELD_PWR2_PVT_RST_EN                     (9UL)
#define PMU_ADDR_RST_ENABLE_FIELD_PWR2_PVT_RST_EN_MASK                (PMU_VA_MASK_1 << PMU_ADDR_RST_ENABLE_FIELD_PWR2_PVT_RST_EN)
#define PMU_ADDR_RST_ENABLE_FIELD_PWR1_PVT_RST_EN                     (8UL)
#define PMU_ADDR_RST_ENABLE_FIELD_PWR1_PVT_RST_EN_MASK                (PMU_VA_MASK_1 << PMU_ADDR_RST_ENABLE_FIELD_PWR1_PVT_RST_EN)
#define PMU_ADDR_RST_ENABLE_FIELD_PWR0_PVT_RST_EN                     (7UL)
#define PMU_ADDR_RST_ENABLE_FIELD_PWR0_PVT_RST_EN_MASK                (PMU_VA_MASK_1 << PMU_ADDR_RST_ENABLE_FIELD_PWR0_PVT_RST_EN)
#define PMU_ADDR_RST_ENABLE_FIELD_XIN_PVT_RST_EN                      (6UL)
#define PMU_ADDR_RST_ENABLE_FIELD_XIN_PVT_RST_EN_MASK                 (PMU_VA_MASK_1 << PMU_ADDR_RST_ENABLE_FIELD_XIN_PVT_RST_EN)
#define PMU_ADDR_RST_ENABLE_FIELD_FMU_FAULT_RST_EN                    (5UL)
#define PMU_ADDR_RST_ENABLE_FIELD_FMU_FAULT_RST_EN_MASK               (PMU_VA_MASK_1 << PMU_ADDR_RST_ENABLE_FIELD_FMU_FAULT_RST_EN)
#define PMU_ADDR_RST_ENABLE_FIELD_HSM_COLD_RST_EN                     (4UL)
#define PMU_ADDR_RST_ENABLE_FIELD_HSM_COLD_RST_EN_MASK                (PMU_VA_MASK_1 << PMU_ADDR_RST_ENABLE_FIELD_HSM_COLD_RST_EN)
#define PMU_ADDR_RST_ENABLE_FIELD_HSM_WARM_RST_EN                     (3UL)
#define PMU_ADDR_RST_ENABLE_FIELD_HSM_WARM_RST_EN_MASK                (PMU_VA_MASK_1 << PMU_ADDR_RST_ENABLE_FIELD_HSM_WARM_RST_EN)
#define PMU_ADDR_RST_ENABLE_FIELD_HSM_WDT_RST_EN                      (2UL)
#define PMU_ADDR_RST_ENABLE_FIELD_HSM_WDT_RST_EN_MASK                 (PMU_VA_MASK_1 << PMU_ADDR_RST_ENABLE_FIELD_HSM_WDT_RST_EN)
#define PMU_ADDR_RST_ENABLE_FIELD_PMU_WDT_RST_EN                      (1UL)
#define PMU_ADDR_RST_ENABLE_FIELD_PMU_WDT_RST_EN_MASK                 (PMU_VA_MASK_1 << PMU_ADDR_RST_ENABLE_FIELD_PMU_WDT_RST_EN)
#define PMU_ADDR_RST_ENABLE_FIELD_WDT_RST_EN                          (0UL)
#define PMU_ADDR_RST_ENABLE_FIELD_WDT_RST_EN_MASK                     (PMU_VA_MASK_1 << PMU_ADDR_RST_ENABLE_FIELD_WDT_RST_EN)

/* FIELD INFO (Register: COLD_RST_REQ) ===========================================*/
#define PMU_ADDR_COLD_RST_REQ_FIELD_COLD_RST_REQ                      (0UL)
#define PMU_ADDR_COLD_RST_REQ_FIELD_COLD_RST_REQ_MASK                 (PMU_VA_MASK_1 << PMU_ADDR_COLD_RST_REQ_FIELD_COLD_RST_REQ)

/* FIELD INFO (Register: WARM_RST_REQ) ===========================================*/
#define PMU_ADDR_WARM_RST_REQ_FIELD_WARM_RST_REQ                      (0UL)
#define PMU_ADDR_WARM_RST_REQ_FIELD_WARM_RST_REQ_MASK                 (PMU_VA_MASK_1 << PMU_ADDR_WARM_RST_REQ_FIELD_WARM_RST_REQ)

/* FIELD INFO (Register: HSM_RSTN_MSK) ===========================================*/
#define PMU_ADDR_HSM_RSTN_MSK_FIELD_COLD_RST_MASK                     (6UL)
#define PMU_ADDR_HSM_RSTN_MSK_FIELD_COLD_RST_MASK_MASK                (PMU_VA_MASK_1 << PMU_ADDR_HSM_RSTN_MSK_FIELD_COLD_RST_MASK)
#define PMU_ADDR_HSM_RSTN_MSK_FIELD_SW_RST_MASK                       (5UL)
#define PMU_ADDR_HSM_RSTN_MSK_FIELD_SW_RST_MASK_MASK                  (PMU_VA_MASK_1 << PMU_ADDR_HSM_RSTN_MSK_FIELD_SW_RST_MASK)
#define PMU_ADDR_HSM_RSTN_MSK_FIELD_HSM_WARM_RST_MASK                 (4UL)
#define PMU_ADDR_HSM_RSTN_MSK_FIELD_HSM_WARM_RST_MASK_MASK            (PMU_VA_MASK_1 << PMU_ADDR_HSM_RSTN_MSK_FIELD_HSM_WARM_RST_MASK)
#define PMU_ADDR_HSM_RSTN_MSK_FIELD_HSM_WDT_RST_MASK                  (3UL)
#define PMU_ADDR_HSM_RSTN_MSK_FIELD_HSM_WDT_RST_MASK_MASK             (PMU_VA_MASK_1 << PMU_ADDR_HSM_RSTN_MSK_FIELD_HSM_WDT_RST_MASK)
#define PMU_ADDR_HSM_RSTN_MSK_FIELD_PVT_RST_MASK                      (2UL)
#define PMU_ADDR_HSM_RSTN_MSK_FIELD_PVT_RST_MASK_MASK                 (PMU_VA_MASK_1 << PMU_ADDR_HSM_RSTN_MSK_FIELD_PVT_RST_MASK)
#define PMU_ADDR_HSM_RSTN_MSK_FIELD_WARM_RST_MASK                     (1UL)
#define PMU_ADDR_HSM_RSTN_MSK_FIELD_WARM_RST_MASK_MASK                (PMU_VA_MASK_1 << PMU_ADDR_HSM_RSTN_MSK_FIELD_WARM_RST_MASK)
#define PMU_ADDR_HSM_RSTN_MSK_FIELD_WDT_RST_MSK                       (0UL)
#define PMU_ADDR_HSM_RSTN_MSK_FIELD_WDT_RST_MSK_MASK                  (PMU_VA_MASK_1 << PMU_ADDR_HSM_RSTN_MSK_FIELD_WDT_RST_MSK)

/* FIELD INFO (Register: HSM_SWRSTN) ===========================================*/
#define PMU_ADDR_HSM_SWRSTN_FIELD_HSM_SWRSTN                          (0UL)
#define PMU_ADDR_HSM_SWRSTN_FIELD_HSM_SWRSTN_MASK                     (PMU_VA_MASK_1 << PMU_ADDR_HSM_SWRSTN_FIELD_HSM_SWRSTN)

/* FIELD INFO (Register: HSM_STATUS) ===========================================*/
#define PMU_ADDR_HSM_STATUS_FIELD_HSM_VERIFY_STATUS4                  (20UL)
#define PMU_ADDR_HSM_STATUS_FIELD_HSM_VERIFY_STATUS4_MASK             (PMU_VA_MASK_12 << PMU_ADDR_HSM_STATUS_FIELD_HSM_VERIFY_STATUS4)
#define PMU_ADDR_HSM_STATUS_FIELD_HSM_VERIFY_STATUS3                  (19UL)
#define PMU_ADDR_HSM_STATUS_FIELD_HSM_VERIFY_STATUS3_MASK             (PMU_VA_MASK_1 << PMU_ADDR_HSM_STATUS_FIELD_HSM_VERIFY_STATUS3)
#define PMU_ADDR_HSM_STATUS_FIELD_HSM_VERIFY_STATUS2                  (18UL)
#define PMU_ADDR_HSM_STATUS_FIELD_HSM_VERIFY_STATUS2_MASK             (PMU_VA_MASK_1 << PMU_ADDR_HSM_STATUS_FIELD_HSM_VERIFY_STATUS2)
#define PMU_ADDR_HSM_STATUS_FIELD_HSM_VERIFY_STATUS1                  (17UL)
#define PMU_ADDR_HSM_STATUS_FIELD_HSM_VERIFY_STATUS1_MASK             (PMU_VA_MASK_1 << PMU_ADDR_HSM_STATUS_FIELD_HSM_VERIFY_STATUS1)
#define PMU_ADDR_HSM_STATUS_FIELD_HSM_VERIFY_STATUS0                  (16UL)
#define PMU_ADDR_HSM_STATUS_FIELD_HSM_VERIFY_STATUS0_MASK             (PMU_VA_MASK_1 << PMU_ADDR_HSM_STATUS_FIELD_HSM_VERIFY_STATUS0)
#define PMU_ADDR_HSM_STATUS_FIELD_HSM_READY                           (2UL)
#define PMU_ADDR_HSM_STATUS_FIELD_HSM_READY_MASK                      (PMU_VA_MASK_1 << PMU_ADDR_HSM_STATUS_FIELD_HSM_READY)
#define PMU_ADDR_HSM_STATUS_FIELD_HSM_LOCKUP                          (1UL)
#define PMU_ADDR_HSM_STATUS_FIELD_HSM_LOCKUP_MASK                     (PMU_VA_MASK_1 << PMU_ADDR_HSM_STATUS_FIELD_HSM_LOCKUP)
#define PMU_ADDR_HSM_STATUS_FIELD_HSM_HALTED                          (0UL)
#define PMU_ADDR_HSM_STATUS_FIELD_HSM_HALTED_MASK                     (PMU_VA_MASK_1 << PMU_ADDR_HSM_STATUS_FIELD_HSM_HALTED)

/* FIELD INFO (Register: SYSRST_CTRL) ===========================================*/
#define PMU_ADDR_SYSRST_CTRL_FIELD_SYSRSTn_REGMODE                    (2UL)
#define PMU_ADDR_SYSRST_CTRL_FIELD_SYSRSTn_REGMODE_MASK               (PMU_VA_MASK_1 << PMU_ADDR_SYSRST_CTRL_FIELD_SYSRSTn_REGMODE)
#define PMU_ADDR_SYSRST_CTRL_FIELD_SYSRSTn_REG                        (1UL)
#define PMU_ADDR_SYSRST_CTRL_FIELD_SYSRSTn_REG_MASK                   (PMU_VA_MASK_1 << PMU_ADDR_SYSRST_CTRL_FIELD_SYSRSTn_REG)
#define PMU_ADDR_SYSRST_CTRL_FIELD_SYSRSTn_MASK                       (0UL)
#define PMU_ADDR_SYSRST_CTRL_FIELD_SYSRSTn_MASK_MASK                  (PMU_VA_MASK_1 << PMU_ADDR_SYSRST_CTRL_FIELD_SYSRSTn_MASK)

/* FIELD INFO (Register: MEM_ECC_CFG) ===========================================*/
#define PMU_ADDR_MEM_ECC_CFG_FIELD_ROM_CRC_EN                         (2UL)
#define PMU_ADDR_MEM_ECC_CFG_FIELD_ROM_CRC_EN_MASK                    (PMU_VA_MASK_1 << PMU_ADDR_MEM_ECC_CFG_FIELD_ROM_CRC_EN)
#define PMU_ADDR_MEM_ECC_CFG_FIELD_RAM_ECC_EN                         (1UL)
#define PMU_ADDR_MEM_ECC_CFG_FIELD_RAM_ECC_EN_MASK                    (PMU_VA_MASK_1 << PMU_ADDR_MEM_ECC_CFG_FIELD_RAM_ECC_EN)
#define PMU_ADDR_MEM_ECC_CFG_FIELD_RAM_INIT_EN                        (0UL)
#define PMU_ADDR_MEM_ECC_CFG_FIELD_RAM_INIT_EN_MASK                   (PMU_VA_MASK_1 << PMU_ADDR_MEM_ECC_CFG_FIELD_RAM_INIT_EN)

/* FIELD INFO (Register: PVT0_CFG) ===========================================*/
#define PMU_ADDR_PVT0_CFG_FIELD_PWR12_MON_EN                          (12UL)
#define PMU_ADDR_PVT0_CFG_FIELD_PWR12_MON_EN_MASK                     (PMU_VA_MASK_1 << PMU_ADDR_PVT0_CFG_FIELD_PWR12_MON_EN)
#define PMU_ADDR_PVT0_CFG_FIELD_PWR11_MON_EN                          (11UL)
#define PMU_ADDR_PVT0_CFG_FIELD_PWR11_MON_EN_MASK                     (PMU_VA_MASK_1 << PMU_ADDR_PVT0_CFG_FIELD_PWR11_MON_EN)
#define PMU_ADDR_PVT0_CFG_FIELD_PWR10_MON_EN                          (10UL)
#define PMU_ADDR_PVT0_CFG_FIELD_PWR10_MON_EN_MASK                     (PMU_VA_MASK_1 << PMU_ADDR_PVT0_CFG_FIELD_PWR10_MON_EN)
#define PMU_ADDR_PVT0_CFG_FIELD_PWR9_MON_EN                           (9UL)
#define PMU_ADDR_PVT0_CFG_FIELD_PWR9_MON_EN_MASK                      (PMU_VA_MASK_1 << PMU_ADDR_PVT0_CFG_FIELD_PWR9_MON_EN)
#define PMU_ADDR_PVT0_CFG_FIELD_PWR8_MON_EN                           (8UL)
#define PMU_ADDR_PVT0_CFG_FIELD_PWR8_MON_EN_MASK                      (PMU_VA_MASK_1 << PMU_ADDR_PVT0_CFG_FIELD_PWR8_MON_EN)
#define PMU_ADDR_PVT0_CFG_FIELD_PWR7_MON_EN                           (7UL)
#define PMU_ADDR_PVT0_CFG_FIELD_PWR7_MON_EN_MASK                      (PMU_VA_MASK_1 << PMU_ADDR_PVT0_CFG_FIELD_PWR7_MON_EN)
#define PMU_ADDR_PVT0_CFG_FIELD_PWR6_MON_EN                           (6UL)
#define PMU_ADDR_PVT0_CFG_FIELD_PWR6_MON_EN_MASK                      (PMU_VA_MASK_1 << PMU_ADDR_PVT0_CFG_FIELD_PWR6_MON_EN)
#define PMU_ADDR_PVT0_CFG_FIELD_PWR5_MON_EN                           (5UL)
#define PMU_ADDR_PVT0_CFG_FIELD_PWR5_MON_EN_MASK                      (PMU_VA_MASK_1 << PMU_ADDR_PVT0_CFG_FIELD_PWR5_MON_EN)
#define PMU_ADDR_PVT0_CFG_FIELD_PWR4_MON_EN                           (4UL)
#define PMU_ADDR_PVT0_CFG_FIELD_PWR4_MON_EN_MASK                      (PMU_VA_MASK_1 << PMU_ADDR_PVT0_CFG_FIELD_PWR4_MON_EN)
#define PMU_ADDR_PVT0_CFG_FIELD_PWR3_MON_EN                           (3UL)
#define PMU_ADDR_PVT0_CFG_FIELD_PWR3_MON_EN_MASK                      (PMU_VA_MASK_1 << PMU_ADDR_PVT0_CFG_FIELD_PWR3_MON_EN)
#define PMU_ADDR_PVT0_CFG_FIELD_PWR2_MON_EN                           (2UL)
#define PMU_ADDR_PVT0_CFG_FIELD_PWR2_MON_EN_MASK                      (PMU_VA_MASK_1 << PMU_ADDR_PVT0_CFG_FIELD_PWR2_MON_EN)
#define PMU_ADDR_PVT0_CFG_FIELD_PWR1_MON_EN                           (1UL)
#define PMU_ADDR_PVT0_CFG_FIELD_PWR1_MON_EN_MASK                      (PMU_VA_MASK_1 << PMU_ADDR_PVT0_CFG_FIELD_PWR1_MON_EN)
#define PMU_ADDR_PVT0_CFG_FIELD_PWR0_MON_EN                           (0UL)
#define PMU_ADDR_PVT0_CFG_FIELD_PWR0_MON_EN_MASK                      (PMU_VA_MASK_1 << PMU_ADDR_PVT0_CFG_FIELD_PWR0_MON_EN)

/* FIELD INFO (Register: PVT1_CFG) ===========================================*/
#define PMU_ADDR_PVT1_CFG_FIELD_PWR12_OUTRANGE                        (12UL)
#define PMU_ADDR_PVT1_CFG_FIELD_PWR12_OUTRANGE_MASK                   (PMU_VA_MASK_1 << PMU_ADDR_PVT1_CFG_FIELD_PWR12_OUTRANGE)
#define PMU_ADDR_PVT1_CFG_FIELD_PWR11_OUTRANGE                        (11UL)
#define PMU_ADDR_PVT1_CFG_FIELD_PWR11_OUTRANGE_MASK                   (PMU_VA_MASK_1 << PMU_ADDR_PVT1_CFG_FIELD_PWR11_OUTRANGE)
#define PMU_ADDR_PVT1_CFG_FIELD_PWR10_OUTRANGE                        (10UL)
#define PMU_ADDR_PVT1_CFG_FIELD_PWR10_OUTRANGE_MASK                   (PMU_VA_MASK_1 << PMU_ADDR_PVT1_CFG_FIELD_PWR10_OUTRANGE)
#define PMU_ADDR_PVT1_CFG_FIELD_PWR9_OUTRANGE                         (9UL)
#define PMU_ADDR_PVT1_CFG_FIELD_PWR9_OUTRANGE_MASK                    (PMU_VA_MASK_1 << PMU_ADDR_PVT1_CFG_FIELD_PWR9_OUTRANGE)
#define PMU_ADDR_PVT1_CFG_FIELD_PWR8_OUTRANGE                         (8UL)
#define PMU_ADDR_PVT1_CFG_FIELD_PWR8_OUTRANGE_MASK                    (PMU_VA_MASK_1 << PMU_ADDR_PVT1_CFG_FIELD_PWR8_OUTRANGE)
#define PMU_ADDR_PVT1_CFG_FIELD_PWR7_OUTRANGE                         (7UL)
#define PMU_ADDR_PVT1_CFG_FIELD_PWR7_OUTRANGE_MASK                    (PMU_VA_MASK_1 << PMU_ADDR_PVT1_CFG_FIELD_PWR7_OUTRANGE)
#define PMU_ADDR_PVT1_CFG_FIELD_PWR6_OUTRANGE                         (6UL)
#define PMU_ADDR_PVT1_CFG_FIELD_PWR6_OUTRANGE_MASK                    (PMU_VA_MASK_1 << PMU_ADDR_PVT1_CFG_FIELD_PWR6_OUTRANGE)
#define PMU_ADDR_PVT1_CFG_FIELD_PWR5_OUTRANGE                         (5UL)
#define PMU_ADDR_PVT1_CFG_FIELD_PWR5_OUTRANGE_MASK                    (PMU_VA_MASK_1 << PMU_ADDR_PVT1_CFG_FIELD_PWR5_OUTRANGE)
#define PMU_ADDR_PVT1_CFG_FIELD_PWR4_OUTRANGE                         (4UL)
#define PMU_ADDR_PVT1_CFG_FIELD_PWR4_OUTRANGE_MASK                    (PMU_VA_MASK_1 << PMU_ADDR_PVT1_CFG_FIELD_PWR4_OUTRANGE)
#define PMU_ADDR_PVT1_CFG_FIELD_PWR3_OUTRANGE                         (3UL)
#define PMU_ADDR_PVT1_CFG_FIELD_PWR3_OUTRANGE_MASK                    (PMU_VA_MASK_1 << PMU_ADDR_PVT1_CFG_FIELD_PWR3_OUTRANGE)
#define PMU_ADDR_PVT1_CFG_FIELD_PWR2_OUTRANGE                         (2UL)
#define PMU_ADDR_PVT1_CFG_FIELD_PWR2_OUTRANGE_MASK                    (PMU_VA_MASK_1 << PMU_ADDR_PVT1_CFG_FIELD_PWR2_OUTRANGE)
#define PMU_ADDR_PVT1_CFG_FIELD_PWR1_OUTRANGE                         (1UL)
#define PMU_ADDR_PVT1_CFG_FIELD_PWR1_OUTRANGE_MASK                    (PMU_VA_MASK_1 << PMU_ADDR_PVT1_CFG_FIELD_PWR1_OUTRANGE)
#define PMU_ADDR_PVT1_CFG_FIELD_PWR0_OUTRANGE                         (0UL)
#define PMU_ADDR_PVT1_CFG_FIELD_PWR0_OUTRANGE_MASK                    (PMU_VA_MASK_1 << PMU_ADDR_PVT1_CFG_FIELD_PWR0_OUTRANGE)

/* FIELD INFO (Register: PVT2_CFG) ===========================================*/
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR22                             (22UL)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR22_MASK                        (PMU_VA_MASK_1 << PMU_ADDR_PVT2_CFG_FIELD_STS_PWR22)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR21                             (21UL)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR21_MASK                        (PMU_VA_MASK_1 << PMU_ADDR_PVT2_CFG_FIELD_STS_PWR21)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR20                             (20UL)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR20_MASK                        (PMU_VA_MASK_1 << PMU_ADDR_PVT2_CFG_FIELD_STS_PWR20)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR19                             (19UL)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR19_MASK                        (PMU_VA_MASK_1 << PMU_ADDR_PVT2_CFG_FIELD_STS_PWR19)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR18                             (18UL)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR18_MASK                        (PMU_VA_MASK_1 << PMU_ADDR_PVT2_CFG_FIELD_STS_PWR18)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR17                             (17UL)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR17_MASK                        (PMU_VA_MASK_1 << PMU_ADDR_PVT2_CFG_FIELD_STS_PWR17)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR16                             (16UL)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR16_MASK                        (PMU_VA_MASK_1 << PMU_ADDR_PVT2_CFG_FIELD_STS_PWR16)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR15                             (15UL)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR15_MASK                        (PMU_VA_MASK_1 << PMU_ADDR_PVT2_CFG_FIELD_STS_PWR15)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR14                             (14UL)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR14_MASK                        (PMU_VA_MASK_1 << PMU_ADDR_PVT2_CFG_FIELD_STS_PWR14)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR13                             (13UL)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR13_MASK                        (PMU_VA_MASK_1 << PMU_ADDR_PVT2_CFG_FIELD_STS_PWR13)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR12                             (12UL)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR12_MASK                        (PMU_VA_MASK_1 << PMU_ADDR_PVT2_CFG_FIELD_STS_PWR12)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR11                             (11UL)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR11_MASK                        (PMU_VA_MASK_1 << PMU_ADDR_PVT2_CFG_FIELD_STS_PWR11)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR10                             (10UL)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR10_MASK                        (PMU_VA_MASK_1 << PMU_ADDR_PVT2_CFG_FIELD_STS_PWR10)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR9                              (9UL)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR9_MASK                         (PMU_VA_MASK_1 << PMU_ADDR_PVT2_CFG_FIELD_STS_PWR9)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR8                              (8UL)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR8_MASK                         (PMU_VA_MASK_1 << PMU_ADDR_PVT2_CFG_FIELD_STS_PWR8)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR7                              (7UL)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR7_MASK                         (PMU_VA_MASK_1 << PMU_ADDR_PVT2_CFG_FIELD_STS_PWR7)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR6                              (6UL)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR6_MASK                         (PMU_VA_MASK_1 << PMU_ADDR_PVT2_CFG_FIELD_STS_PWR6)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR5                              (5UL)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR5_MASK                         (PMU_VA_MASK_1 << PMU_ADDR_PVT2_CFG_FIELD_STS_PWR5)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR4                              (4UL)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR4_MASK                         (PMU_VA_MASK_1 << PMU_ADDR_PVT2_CFG_FIELD_STS_PWR4)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR3                              (3UL)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR3_MASK                         (PMU_VA_MASK_1 << PMU_ADDR_PVT2_CFG_FIELD_STS_PWR3)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR2                              (2UL)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR2_MASK                         (PMU_VA_MASK_1 << PMU_ADDR_PVT2_CFG_FIELD_STS_PWR2)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR1                              (1UL)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR1_MASK                         (PMU_VA_MASK_1 << PMU_ADDR_PVT2_CFG_FIELD_STS_PWR1)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR0                              (0UL)
#define PMU_ADDR_PVT2_CFG_FIELD_STS_PWR0_MASK                         (PMU_VA_MASK_1 << PMU_ADDR_PVT2_CFG_FIELD_STS_PWR0)

/* FIELD INFO (Register: PVT3_CFG) ===========================================*/
#define PMU_ADDR_PVT3_CFG_FIELD_CFG_PWR_MON_THR                       (4UL)
#define PMU_ADDR_PVT3_CFG_FIELD_CFG_PWR_MON_THR_MASK                  (PMU_VA_MASK_6 << PMU_ADDR_PVT3_CFG_FIELD_CFG_PWR_MON_THR)
#define PMU_ADDR_PVT3_CFG_FIELD_CFG_PWR_MON_SETTLE                    (0UL)
#define PMU_ADDR_PVT3_CFG_FIELD_CFG_PWR_MON_SETTLE_MASK               (PMU_VA_MASK_3 << PMU_ADDR_PVT3_CFG_FIELD_CFG_PWR_MON_SETTLE)

/* FIELD INFO (Register: PVT4_CFG) ===========================================*/
#define PMU_ADDR_PVT4_CFG_FIELD_XIN_MON_EN                            (16UL)
#define PMU_ADDR_PVT4_CFG_FIELD_XIN_MON_EN_MASK                       (PMU_VA_MASK_1 << PMU_ADDR_PVT4_CFG_FIELD_XIN_MON_EN)
#define PMU_ADDR_PVT4_CFG_FIELD_CFG_XIN_MON_SETTLE                    (13UL)
#define PMU_ADDR_PVT4_CFG_FIELD_CFG_XIN_MON_SETTLE_MASK               (PMU_VA_MASK_3 << PMU_ADDR_PVT4_CFG_FIELD_CFG_XIN_MON_SETTLE)
#define PMU_ADDR_PVT4_CFG_FIELD_XIN_OURANGE                           (12UL)
#define PMU_ADDR_PVT4_CFG_FIELD_XIN_OURANGE_MASK                      (PMU_VA_MASK_1 << PMU_ADDR_PVT4_CFG_FIELD_XIN_OURANGE)
#define PMU_ADDR_PVT4_CFG_FIELD_OSC_XIN_CAPTURE                       (8UL)
#define PMU_ADDR_PVT4_CFG_FIELD_OSC_XIN_CAPTURE_MASK                  (PMU_VA_MASK_4 << PMU_ADDR_PVT4_CFG_FIELD_OSC_XIN_CAPTURE)
#define PMU_ADDR_PVT4_CFG_FIELD_OSC_XIN_RANGE                         (0UL)
#define PMU_ADDR_PVT4_CFG_FIELD_OSC_XIN_RANGE_MASK                    (PMU_VA_MASK_8 << PMU_ADDR_PVT4_CFG_FIELD_OSC_XIN_RANGE)

/* FIELD INFO (Register: PVT_SM_CFG) ===========================================*/
#define PMU_ADDR_PVT_SM_CFG_FIELD_POWER_SM_FMU_EN                     (3UL)
#define PMU_ADDR_PVT_SM_CFG_FIELD_POWER_SM_FMU_EN_MASK                (PMU_VA_MASK_1 << PMU_ADDR_PVT_SM_CFG_FIELD_POWER_SM_FMU_EN)
#define PMU_ADDR_PVT_SM_CFG_FIELD_POWER_PERIODIC_EN                   (2UL)
#define PMU_ADDR_PVT_SM_CFG_FIELD_POWER_PERIODIC_EN_MASK              (PMU_VA_MASK_1 << PMU_ADDR_PVT_SM_CFG_FIELD_POWER_PERIODIC_EN)
#define PMU_ADDR_PVT_SM_CFG_FIELD_XIN_SM_FMU_EN                       (1UL)
#define PMU_ADDR_PVT_SM_CFG_FIELD_XIN_SM_FMU_EN_MASK                  (PMU_VA_MASK_1 << PMU_ADDR_PVT_SM_CFG_FIELD_XIN_SM_FMU_EN)
#define PMU_ADDR_PVT_SM_CFG_FIELD_XIN_PERIODIC_EN                     (0UL)
#define PMU_ADDR_PVT_SM_CFG_FIELD_XIN_PERIODIC_EN_MASK                (PMU_VA_MASK_1 << PMU_ADDR_PVT_SM_CFG_FIELD_XIN_PERIODIC_EN)

/* FIELD INFO (Register: XIN_SELECT_FREQ) ===========================================*/
#define PMU_ADDR_XIN_SELECT_FREQ_FIELD_XIN_SF1                        (1UL)
#define PMU_ADDR_XIN_SELECT_FREQ_FIELD_XIN_SF1_MASK                   (PMU_VA_MASK_1 << PMU_ADDR_XIN_SELECT_FREQ_FIELD_XIN_SF1)
#define PMU_ADDR_XIN_SELECT_FREQ_FIELD_XIN_SF0                        (0UL)
#define PMU_ADDR_XIN_SELECT_FREQ_FIELD_XIN_SF0_MASK                   (PMU_VA_MASK_1 << PMU_ADDR_XIN_SELECT_FREQ_FIELD_XIN_SF0)

/* FIELD INFO (Register: OSC_CFG) ===========================================*/
#define PMU_ADDR_OSC_CFG_FIELD_OSC_EN                                 (8UL)
#define PMU_ADDR_OSC_CFG_FIELD_OSC_EN_MASK                            (PMU_VA_MASK_1 << PMU_ADDR_OSC_CFG_FIELD_OSC_EN)
#define PMU_ADDR_OSC_CFG_FIELD_OSC_FSEL                               (0UL)
#define PMU_ADDR_OSC_CFG_FIELD_OSC_FSEL_MASK                          (PMU_VA_MASK_8 << PMU_ADDR_OSC_CFG_FIELD_OSC_FSEL)

/* FIELD INFO (Register: ECID_USER0_FBOUT0) ===========================================*/
#define PMU_ADDR_ECID_USER0_FBOUT0_FIELD_ECID_USER0_FBOUT             (0UL)
#define PMU_ADDR_ECID_USER0_FBOUT0_FIELD_ECID_USER0_FBOUT_MASK        (PMU_VA_MASK_32 << PMU_ADDR_ECID_USER0_FBOUT0_FIELD_ECID_USER0_FBOUT)

/* FIELD INFO (Register: ECID_USER0_FBOUT1) ===========================================*/
#define PMU_ADDR_ECID_USER0_FBOUT1_FIELD_ECID_USER0_FBOUT             (0UL)
#define PMU_ADDR_ECID_USER0_FBOUT1_FIELD_ECID_USER0_FBOUT_MASK        (PMU_VA_MASK_32 << PMU_ADDR_ECID_USER0_FBOUT1_FIELD_ECID_USER0_FBOUT)

/* FIELD INFO (Register: ECID_USER0_FBOUT2) ===========================================*/
#define PMU_ADDR_ECID_USER0_FBOUT2_FIELD_ECID_USER0_FBOUT_DONE        (31UL)
#define PMU_ADDR_ECID_USER0_FBOUT2_FIELD_ECID_USER0_FBOUT_DONE_MASK   (PMU_VA_MASK_1 << PMU_ADDR_ECID_USER0_FBOUT2_FIELD_ECID_USER0_FBOUT_DONE)
#define PMU_ADDR_ECID_USER0_FBOUT2_FIELD_ECID_USER0_FBOUT             (0UL)
#define PMU_ADDR_ECID_USER0_FBOUT2_FIELD_ECID_USER0_FBOUT_MASK        (PMU_VA_MASK_16 << PMU_ADDR_ECID_USER0_FBOUT2_FIELD_ECID_USER0_FBOUT)

/* FIELD INFO (Register: ECID_USER1_FBOUT0) ===========================================*/
#define PMU_ADDR_ECID_USER1_FBOUT0_FIELD_ECID_USER1_FBOUT             (0UL)
#define PMU_ADDR_ECID_USER1_FBOUT0_FIELD_ECID_USER1_FBOUT_MASK        (PMU_VA_MASK_32 << PMU_ADDR_ECID_USER1_FBOUT0_FIELD_ECID_USER1_FBOUT)

/* FIELD INFO (Register: ECID_USER1_FBOUT1) ===========================================*/
#define PMU_ADDR_ECID_USER1_FBOUT1_FIELD_ECID_USER1_FBOUT             (0UL)
#define PMU_ADDR_ECID_USER1_FBOUT1_FIELD_ECID_USER1_FBOUT_MASK        (PMU_VA_MASK_32 << PMU_ADDR_ECID_USER1_FBOUT1_FIELD_ECID_USER1_FBOUT)

/* FIELD INFO (Register: ECID_USER1_FBOUT2) ===========================================*/
#define PMU_ADDR_ECID_USER1_FBOUT2_FIELD_ECID_USER1_FBOUT_DONE        (31UL)
#define PMU_ADDR_ECID_USER1_FBOUT2_FIELD_ECID_USER1_FBOUT_DONE_MASK   (PMU_VA_MASK_1 << PMU_ADDR_ECID_USER1_FBOUT2_FIELD_ECID_USER1_FBOUT_DONE)
#define PMU_ADDR_ECID_USER1_FBOUT2_FIELD_ECID_USER1_FBOUT             (0UL)
#define PMU_ADDR_ECID_USER1_FBOUT2_FIELD_ECID_USER1_FBOUT_MASK        (PMU_VA_MASK_16 << PMU_ADDR_ECID_USER1_FBOUT2_FIELD_ECID_USER1_FBOUT)

/* FIELD INFO (Register: MEM_CFG) ===========================================*/
#define PMU_ADDR_MEM_CFG_FIELD_MCS_RA1_HD                             (13UL)
#define PMU_ADDR_MEM_CFG_FIELD_MCS_RA1_HD_MASK                        (PMU_VA_MASK_2 << PMU_ADDR_MEM_CFG_FIELD_MCS_RA1_HD)
#define PMU_ADDR_MEM_CFG_FIELD_MCSW_RA1_HS                            (12UL)
#define PMU_ADDR_MEM_CFG_FIELD_MCSW_RA1_HS_MASK                       (PMU_VA_MASK_1 << PMU_ADDR_MEM_CFG_FIELD_MCSW_RA1_HS)
#define PMU_ADDR_MEM_CFG_FIELD_MCS_RA1_HS                             (10UL)
#define PMU_ADDR_MEM_CFG_FIELD_MCS_RA1_HS_MASK                        (PMU_VA_MASK_2 << PMU_ADDR_MEM_CFG_FIELD_MCS_RA1_HS)
#define PMU_ADDR_MEM_CFG_FIELD_ADME_RA1_HS                            (8UL)
#define PMU_ADDR_MEM_CFG_FIELD_ADME_RA1_HS_MASK                       (PMU_VA_MASK_2 << PMU_ADDR_MEM_CFG_FIELD_ADME_RA1_HS)
#define PMU_ADDR_MEM_CFG_FIELD_MCS_VROMP_HD                           (6UL)
#define PMU_ADDR_MEM_CFG_FIELD_MCS_VROMP_HD_MASK                      (PMU_VA_MASK_2 << PMU_ADDR_MEM_CFG_FIELD_MCS_VROMP_HD)
#define PMU_ADDR_MEM_CFG_FIELD_KCS_VROMP_HD                           (5UL)
#define PMU_ADDR_MEM_CFG_FIELD_KCS_VROMP_HD_MASK                      (PMU_VA_MASK_1 << PMU_ADDR_MEM_CFG_FIELD_KCS_VROMP_HD)
#define PMU_ADDR_MEM_CFG_FIELD_MCSRD_RF2_HS                           (3UL)
#define PMU_ADDR_MEM_CFG_FIELD_MCSRD_RF2_HS_MASK                      (PMU_VA_MASK_2 << PMU_ADDR_MEM_CFG_FIELD_MCSRD_RF2_HS)
#define PMU_ADDR_MEM_CFG_FIELD_MCSWR_RF2_HS                           (1UL)
#define PMU_ADDR_MEM_CFG_FIELD_MCSWR_RF2_HS_MASK                      (PMU_VA_MASK_2 << PMU_ADDR_MEM_CFG_FIELD_MCSWR_RF2_HS)
#define PMU_ADDR_MEM_CFG_FIELD_KCS_RF2_HS                             (0UL)
#define PMU_ADDR_MEM_CFG_FIELD_KCS_RF2_HS_MASK                        (PMU_VA_MASK_1 << PMU_ADDR_MEM_CFG_FIELD_KCS_RF2_HS)

/* FIELD INFO (Register: BACKUP_REG) ===========================================*/
#define PMU_ADDR_BACKUP_REG_FIELD_BACKUP_REG                          (0UL)
#define PMU_ADDR_BACKUP_REG_FIELD_BACKUP_REG_MASK                     (PMU_VA_MASK_32 << PMU_ADDR_BACKUP_REG_FIELD_BACKUP_REG)

/* FIELD INFO (Register: PMU_WDT_EN) ===========================================*/
#define PMU_ADDR_PMU_WDT_EN_FIELD_WDT_EN                              (0UL)
#define PMU_ADDR_PMU_WDT_EN_FIELD_WDT_EN_MASK                         (PMU_VA_MASK_1 << PMU_ADDR_PMU_WDT_EN_FIELD_WDT_EN)

/* FIELD INFO (Register: PMU_WDT_CLR) ===========================================*/
#define PMU_ADDR_PMU_WDT_CLR_FIELD_WDT_RST_CLR                        (1UL)
#define PMU_ADDR_PMU_WDT_CLR_FIELD_WDT_RST_CLR_MASK                   (PMU_VA_MASK_1 << PMU_ADDR_PMU_WDT_CLR_FIELD_WDT_RST_CLR)
#define PMU_ADDR_PMU_WDT_CLR_FIELD_WDT_CLR                            (0UL)
#define PMU_ADDR_PMU_WDT_CLR_FIELD_WDT_CLR_MASK                       (PMU_VA_MASK_1 << PMU_ADDR_PMU_WDT_CLR_FIELD_WDT_CLR)

/* FIELD INFO (Register: PMU_WDT_IRQ_CNT) ===========================================*/
#define PMU_ADDR_PMU_WDT_IRQ_CNT_FIELD_WDT_IRQ_VALUE                  (0UL)
#define PMU_ADDR_PMU_WDT_IRQ_CNT_FIELD_WDT_IRQ_VALUE_MASK             (PMU_VA_MASK_32 << PMU_ADDR_PMU_WDT_IRQ_CNT_FIELD_WDT_IRQ_VALUE)

/* FIELD INFO (Register: PMU_WDT_RST_CNT) ===========================================*/
#define PMU_ADDR_PMU_WDT_RST_CNT_FIELD_WDT_RST_CNT                    (0UL)
#define PMU_ADDR_PMU_WDT_RST_CNT_FIELD_WDT_RST_CNT_MASK               (PMU_VA_MASK_32 << PMU_ADDR_PMU_WDT_RST_CNT_FIELD_WDT_RST_CNT)

/* FIELD INFO (Register: PMU_WDT_SM_MODE) ===========================================*/
#define PMU_ADDR_PMU_WDT_SM_MODE_FIELD_CONT                           (3UL)
#define PMU_ADDR_PMU_WDT_SM_MODE_FIELD_CONT_MASK                      (PMU_VA_MASK_1 << PMU_ADDR_PMU_WDT_SM_MODE_FIELD_CONT)
#define PMU_ADDR_PMU_WDT_SM_MODE_FIELD_RST_SEL                        (2UL)
#define PMU_ADDR_PMU_WDT_SM_MODE_FIELD_RST_SEL_MASK                   (PMU_VA_MASK_1 << PMU_ADDR_PMU_WDT_SM_MODE_FIELD_RST_SEL)
#define PMU_ADDR_PMU_WDT_SM_MODE_FIELD_2oo3                           (0UL)
#define PMU_ADDR_PMU_WDT_SM_MODE_FIELD_2oo3_MASK                      (PMU_VA_MASK_2 << PMU_ADDR_PMU_WDT_SM_MODE_FIELD_2oo3)

/* FIELD INFO (Register: PMU_WDT_LOCK) ===========================================*/
#define PMU_ADDR_PMU_WDT_LOCK_FIELD_PMU_WDT_LOCK                      (0UL)
#define PMU_ADDR_PMU_WDT_LOCK_FIELD_PMU_WDT_LOCK_MASK                 (PMU_VA_MASK_32 << PMU_ADDR_PMU_WDT_LOCK_FIELD_PMU_WDT_LOCK)

/* FIELD INFO (Register: PMU_WR_PW) ===========================================*/
#define PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW                            (0UL)
#define PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW_MASK                       (PMU_VA_MASK_32 << PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW)


/*
***************************************************************************************************
*                                             LOCAL VARIABLES
***************************************************************************************************
*/


/*
***************************************************************************************************
*                                         FUNCTION PROTOTYPES
***************************************************************************************************
*/

#endif //MCU_BSP_PMU_REG_HEADER
