#ifndef __GF_SPI_H
#define __GF_SPI_H

#include <linux/types.h>
#include <linux/notifier.h>
/**********************************************************/

enum gf_error_type {
	GF_NO_ERROR = 0,
	GF_PERM_ERROR = 1,
};

#define GF_DEVICE_AVAILABLE 1
#define GF_DEVICE_NOT_AVAILABLE 0

#define GF_SPI_CLK_ENABLED 1
#define GF_SPI_CLK_DISABLED 0

//#define AP_CONTROL_CLK      0
#define GF_NETLINK_ENABLE   1

#define GF_NET_EVENT_IRQ 0
#define GF_NET_EVENT_FB_BLACK 1
#define GF_NET_EVENT_FB_UNBLACK 2
#define GF_DEFAULT_SPEED 4800000


/****************Chip Specific***********************/
#define GF_W          	0xF0
#define GF_R          	0xF1
#define GF_WDATA_OFFSET	(0x3)
#define GF_RDATA_OFFSET	(0x4)

struct gf_configs {
	unsigned short addr;
	unsigned short value;
};

struct gf_mode_config {
	struct gf_configs *p_cfg;
	unsigned int cfg_len;
};

enum gf_spi_transfer_speed {
	GF_SPI_LOW_SPEED = 0,
	GF_SPI_HIGH_SPEED,
	GF_SPI_KEEP_SPEED,
};

#define  GF_IOC_MAGIC         'G'
#define  GF_IOC_DISABLE_IRQ	_IO(GF_IOC_MAGIC, 0)
#define  GF_IOC_ENABLE_IRQ	_IO(GF_IOC_MAGIC, 1)
#define  GF_IOC_SETSPEED    _IOW(GF_IOC_MAGIC, 2, unsigned int)
#define  GF_IOC_RESET       _IO(GF_IOC_MAGIC, 3)
#define  GF_IOC_COOLBOOT    _IO(GF_IOC_MAGIC, 4)
#define  GF_IOC_SENDKEY    _IOW(GF_IOC_MAGIC, 5, struct gf_key)
#define  GF_IOC_CLK_READY  _IO(GF_IOC_MAGIC, 6)
#define  GF_IOC_CLK_UNREADY  _IO(GF_IOC_MAGIC, 7)
#define  GF_IOC_PM_FBCABCK  _IO(GF_IOC_MAGIC, 8)
#define  GF_IOC_POWER_ON   _IO(GF_IOC_MAGIC, 9)
#define  GF_IOC_POWER_OFF  _IO(GF_IOC_MAGIC, 10)
#define  GF_IOC_MAXNR    11

#define TZBSP_APSS_ID                   3
#define TZBSP_TZ_ID                     1

#define GF_DEBUG
/*#undef  GF_DEBUG*/

#ifdef  GF_DEBUG
#define gf_dbg(fmt, args...) do { \
	pr_warn("[gf]" fmt, ##args);\
} while (0)
#define FUNC_ENTRY()  pr_warn("[gf]%s, entry\n", __func__)
#define FUNC_EXIT()  pr_warn("[gf]%s, exit\n", __func__)
#else
#define gf_dbg(fmt, args...)
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

struct gf_ioc_transfer {
	unsigned char cmd;
	unsigned char reserve;
	unsigned short addr;
	unsigned int len;
	unsigned char* buf;
};

struct gf_key {
	unsigned int key;
	int value;
};

struct gf_key_map
{
	char *name;
	unsigned short val;
};

struct gf_dev {
	dev_t devt;
	spinlock_t   spi_lock;
	struct list_head device_entry;
	struct spi_device *spi;
	struct clk *core_clk;
	struct clk *iface_clk;

	struct input_dev *input;
	/* buffer is NULL unless this device is open (users > 0) */
	struct input_dev *lge_input;
	struct device *device;
	struct regulator *vreg;
	u32 qup_id;
	bool pipe_owner;
	bool power_on;
	unsigned users;
	signed irq_gpio;
	signed reset_gpio;
	signed cs_gpio;
	signed pwr_gpio;
	int irq;
	int irq_enabled;
	int clk_enabled;
#ifdef GF_FASYNC
	struct fasync_struct *async;
#endif
	struct notifier_block notifier;
	char device_available;
	char fb_black;
	unsigned char *gBuffer;
	struct mutex buf_lock;
	struct mutex frame_lock;
};

int  gf_parse_dts(struct gf_dev* gf_dev);
void gf_cleanup(struct gf_dev *gf_dev);

int  gf_power_on(struct gf_dev *gf_dev);
int  gf_power_off(struct gf_dev *gf_dev);

int  gf_hw_reset(struct gf_dev *gf_dev, unsigned int delay_ms);
int  gf_irq_num(struct gf_dev *gf_dev);

void sendnlmsg(char *message);
int netlink_init(void);
void netlink_exit(void);


#endif /*__GF_SPI_H*/
