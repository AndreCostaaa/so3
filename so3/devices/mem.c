#include <vfs.h>
#include <common.h>
#include <process.h>
#include <asm/io.h>
#include <asm/mmu.h>
#include <device/driver.h>

#define DEV_CLASS_MEM "mem"

static void *mem_mmap(int fd, addr_t virt_addr, uint32_t page_count, off_t offset)
{
	pcb_t *pcb;
	bool no_cache;

	pcb = current()->pcb;

	/* Use cache only if the target offset is in RAM */
	no_cache = (offset < mem_info.phys_base) &&
	           ((mem_info.phys_base + mem_info.size) <= offset);

	create_mapping(pcb->pgtable, virt_addr, offset, page_count * PAGE_SIZE, no_cache);

	return (void *)virt_addr;
}

static struct file_operations mem_fops = {
	.mmap = mem_mmap,
};

static struct devclass mem_cdev = {
	.class = DEV_CLASS_MEM,
	.type = VFS_TYPE_DEV_CHAR,
	.fops = &mem_fops,
};

static int mem_init(dev_t *dev, int fdt_offset)
{
	devclass_register(dev, &mem_cdev);
	return 0;
}
REGISTER_DRIVER_POSTCORE("mem", mem_init);
