#
# Makefile for the linux kernel.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).

# Object file lists.

obj-y			:= arch.o irq.o timer.o gpio.o

obj-$(CONFIG_ARCH_UC7112)	+= apb_dma.o
obj-$(CONFIG_ARCH_EM1220)	+= apb_dma.o
obj-$(CONFIG_ARCH_EM1220_DLIN)	+= apb_dma.o
obj-$(CONFIG_ARCH_EM1240)	+= apb_dma.o
obj-$(CONFIG_ARCH_EM1240_IVTC)	+= apb_dma.o
obj-$(CONFIG_ARCH_EM1220_APIT)	+= apb_dma.o
obj-$(CONFIG_ARCH_EM1240_MT)	+= apb_dma.o
obj-$(CONFIG_ARCH_UC7101)	+= apb_dma.o
obj-$(CONFIG_ARCH_W311_TEST)	+= apb_dma.o
obj-$(CONFIG_PCI)		+= pci.o
