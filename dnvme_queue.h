#ifndef _DNVME_QUEUE_H_
#define _DNVME_QUEUE_H_

#include "dnvme_reg.h"
#include "dnvme_ds.h"

/* Admin SQ tail Door bell offset */
#define NVME_SQ0TBDL    0x1000

/* Admin SQ tail Door bell offset */
#define NVME_CQ0TBDL    0x1004

/* Admin SQ size Mask bits 0-11 in AQA */
#define ASQS_MASK       0xFFF

/* Admin Completion Q Mask Bits 16-21 in ADA */
#define ACQS_MASK       0x0FFF0000

/* As Time Out is in lower 32 bits of 64 bit CAP */
#define NVME_TO_MASK    0xFF000000

/*
 * Maximum AQ entries allowed.
 */
#define MAX_AQ_ENTRIES   4096

/*
* This lines are commented to use inline functions.
* if required at multiple places uncomment this.
*/
#if 0
#ifdef QEMU
#define WRITEQ(a, b) { writel(a, b); writel(a >> 32, b + 4); }
#else
#define WRITEQ(a, b) { writeq(a, b); }
#endif
#endif
/*
* Enumerating the different NVME Controller Capabilities of the
* PCI Express device as per NVME Spec 1.0b.
*/
enum {
    NVME_CC_ENABLE        = 1 << 0,
    NVME_CC_CSS_NVM       = 0 << 4,
    NVME_CC_MPS_SHIFT     = 7,
    NVME_CC_ARB_RR        = 0 << 11,
    NVME_CC_ARB_WRRU      = 1 << 11,
    NVME_CC_ARB_VS        = 3 << 11,
    NVME_CC_SHN_NONE      = 0 << 13,
    NVME_CC_SHN_NORMAL    = 1 << 13,
    NVME_CC_SHN_ABRUPT    = 2 << 13,
    NVME_CSTS_RDY         = 1 << 0,
    NVME_CSTS_CFS         = 1 << 1,
    NVME_CSTS_SHST_NORMAL = 0 << 2,
    NVME_CSTS_SHST_OCCUR  = 1 << 2,
    NVME_CSTS_SHST_CMPLT  = 2 << 2,
};

/*
 * completion q entry structure.
 */
struct cq_completion {
    u32 cmd_specifc;       /* DW 0 all 32 bits     */
    u32 reserved;          /* DW 1 all 32 bits     */
    u16 sq_head_ptr;       /* DW 2 lower 16 bits   */
    u16 sq_identifier;     /* DW 2 higher 16 bits  */
    u16 cmd_identifier;    /* Cmd identifier       */
    u8  phase_bit:1;       /* Phase bit            */
    u16 status_field:15;   /* Status field         */
};

/**
* The user selection of IOCTL for creating admin cq eventually calls
* this function if init is successful. This will create infrastructure
* for Admin Completion Q creation
* @param pnvme_dev
* @param qsize
* @param pmetrics_cq_list
* @return whether ACQ creation successful or not.
*/
int create_admn_cq(struct nvme_device *pnvme_dev, u16 qsize,
        struct  metrics_cq  *pmetrics_cq_list);

/**
* The user selection of IOCTL for creating admin sq eventually calls
* this function if init is successful. This will create infrastructure
* for Admin Submission Q creation
* @param pnvme_dev
* @param qsize
* @param pmetrics_sq_list
* @return whether ASQ creation successful or not.
*/
int create_admn_sq(struct nvme_device *pnvme_dev, u16 qsize,
        struct  metrics_sq  *pmetrics_sq_list);

/**
* nvme_ctrl_enable - NVME controller enable function.This will set the CAP.EN
* flag and this function which call the timer handler and check for the timer
* expiration. It returns success if the ctrl in rdy before timeout.
* @param  pmetrics_device_element
* @return SUCCESS or FAIL
*/
int nvme_ctrl_enable(struct  metrics_device_list *pmetrics_device_element);

/**
* nvme_ctrl_disable - NVME controller disable function.This will reset the
* CAP.EN flag and this function which call the timer handler and check for
* the timer expiration. It returns success if the ctrl in rdy before timeout.
* @param pmetrics_device_element
* @return SUCCESS or FAIL
*/
int nvme_ctrl_disable(struct  metrics_device_list *pmetrics_device_element);

/**
* identify_unique - verify if the q_id specified is unique. If not unique then
* return fail.
* @param q_id
* @param type
* @param pmetrics_device_element
* @return SUCCESS or FAIL
*/
int identify_unique(u16 q_id, enum metrics_type type,
        struct  metrics_device_list *pmetrics_device_element);

/**
* nvme_prepare_sq - NVME controller prepare sq function. This will check
* if q is allocated and then create a memory for the IO SQ.
* @param pmetrics_sq_list
* @param pnvme_dev
* @return SUCCESS or FAIL
*/
int nvme_prepare_sq(struct  metrics_sq  *pmetrics_sq_list,
            struct nvme_device *pnvme_dev);

/**
* nvme_prepare_cq - NVME controller prepare cq function. This will check
* if q is allocated and then create a memory for the IO SQ.
* @param pmetrics_cq_list
* @param pnvme_dev
* @return SUCCESS or FAIL
*/
int nvme_prepare_cq(struct  metrics_cq  *pmetrics_cq_list,
            struct nvme_device *pnvme_dev);

/**
* nvme_ring_sqx_dbl - NVME controller function to ring the appropriate
* SQ doorbell.
* @param ring_sqx
* @param pmetrics_device_element
* @return SUCCESS or FAIL
*/
int nvme_ring_sqx_dbl(u16 *ring_sqx, struct  metrics_device_list
        *pmetrics_device_element);

/**
 * finds the sq node in the given sq list for the given sq id
 * @param pmetrics_device_element
 * @param sq_id
 * @return pointer to the sq node for given sq id
 */
struct metrics_sq *find_sq(struct  metrics_device_list
        *pmetrics_device_element, u16 sq_id);

/**
 * finds the cq node in the given cq list for the given cq id
 * @param pmetrics_device_element
 * @param cq_id
 * @return pointer to the cq node for given cq id
 */
struct metrics_cq *find_cq(struct  metrics_device_list
        *pmetrics_device_element, u16 cq_id);

/*
 * Find the command node for the given sq node and cmd id.
 * @param pmetrics_sq_node
 * @param cmd_id
 * @return pointer to cmd node for given cmd id.
 */
struct cmd_track *find_cmd(struct metrics_sq *pmetrics_sq_node, u16 cmd_id);

#endif
