/*
 * Configuration for the ZCU104 root cell. Copied from zynqmp-zcu102.c
 * and changed where necessary.
 *
 * Copyright (c) Siemens AG, 2016-2019
 */

#include <jailhouse/types.h>
#include <jailhouse/cell-config.h>

struct {
	struct jailhouse_system header;
	__u64 cpus[1];
	struct jailhouse_memory mem_regions[6];
	struct jailhouse_irqchip irqchips[1];
	struct jailhouse_pci_device pci_devices[1];
} __attribute__((packed)) config = {

	.header = {
		.signature = JAILHOUSE_SYSTEM_SIGNATURE,
		.revision = JAILHOUSE_CONFIG_REVISION,
		.flags = JAILHOUSE_SYS_VIRTUAL_DEBUG_CONSOLE,
		.hypervisor_memory = {
			.phys_start = 0x7fc00000,
			.size =       0x00400000,
		},
		.debug_console = {
			.address = 0xff000000,
			.size = 0x1000,
			.type = JAILHOUSE_CON_TYPE_XUARTPS,
			.flags = JAILHOUSE_CON_ACCESS_MMIO |
				 JAILHOUSE_CON_REGDIST_4,
		},
		.platform_info = {
			.pci_mmconfig_base = 0xfc000000,
			.pci_mmconfig_end_bus = 0,
			.pci_is_virtual = 1,
			.arm = {
				.gic_version = 2,
				.gicd_base = 0xf9010000, /*GIC (Generic Interrupt Controller) distributor register base*/
				.gicc_base = 0xf902f000, /*GIC cpu interface register base*/
				.gich_base = 0xf9040000, /*GIC virtual interface control register base*/
				.gicv_base = 0xf906f000, /*GIC virtual cpu interface register base*/
				.maintenance_irq = 25,
			},
		},

	.root_cell = {
		.name = "ZCU104-root",

			.cpu_set_size = sizeof(config.cpus),
			.num_memory_regions = ARRAY_SIZE(config.mem_regions),
			.num_irqchips = ARRAY_SIZE(config.irqchips),
			.num_pci_devices = ARRAY_SIZE(config.pci_devices),

			.vpci_irq_base = 136-32,
		},
	},

	.cpus = {
		0xf,
	},
	
	.mem_regions = {
		/* IVSHMEM shared memory regions*/
		// state region
		{
			.phys_start = 0x7fb00000,
			.virt_start = 0x7fb00000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ,
		},
		// R/W region
		{
			.phys_start = 0x7fb00000 + 0x1000,
			.virt_start = 0x7fb00000 + 0x1000,
			.size = 0x4000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE,
		},
		//OUTPUT ROOT
		{ 
			.phys_start = 0x7fb00000 + 0x5000,
			.virt_start = 0x7fb00000 + 0x5000,
			.size = 0x4000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE,		
		 },
		//OUTPUT INMATE
		{ 
			.phys_start = 0x7fb00000 + 0x9000,
			.virt_start = 0x7fb00000 + 0x9000,
			.size = 0x4000,
			.flags = JAILHOUSE_MEM_READ,
			
		 },

		/* MMIO (permissive) */ {
			.phys_start = 0xfd000000,
			.virt_start = 0xfd000000,
			.size =	      0x03000000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		/* RAM */ {
			.phys_start = 0x00000000,
			.virt_start = 0x00000000,
			.size =       0x7fb00000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE,
		},
	},

	.irqchips = {
		/* GIC */ {
			.address = 0xf9010000,
			.pin_base = 32,
			.pin_bitmap = {
				0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
			},
		},
	},

	.pci_devices = {
		/* 0001:00:01.0 */ {
			.type = JAILHOUSE_PCI_TYPE_IVSHMEM,
			.domain = 1,
			.bdf = 1 << 3,
			.bar_mask = JAILHOUSE_IVSHMEM_BAR_MASK_INTX,
			.shmem_regions_start = 0,
			.shmem_dev_id = 0,
			.shmem_peers = 2,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_UNDEFINED,
		},
	}, 
};

