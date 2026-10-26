#define ELF_SHSTRNDX_LEN	0x1
#define ELF_SHENUM_LEN		0x2
#define ELF_SHENSIZE_LEN	0x2
#define ELF_SHENTRY_LEN		0x8
#define ELF_PHENUM_LEN		0x2
#define ELF_PHESIZE_LEN		0x2
#define ELF_PHENTRY_LEN		0x8
#define ELF_ESIZE_LEN		0x2
#define ELF_FLAGS_LEN		0x4
#define ELF_ENTRY_LEN		0x8
#define ELF_VERSION_LEN		0x4
#define ELF_ARCH_LEN		0x2
#define ELF_TYPE_LEN		0x2
#define ELF_PADDING_LEN		0x8
#define ELF_ABI_LEN			0x1
#define ELF_EVERSION_LEN 	0x1
#define ELF_DATA_LEN		0x1

#define E_IDENT 0x00
#define E_CLASS 0x04
#define E_PHOFF 0x20
#define E_PHOFF_LEN 0x08
#define E_PHENM 0x38
#define E_PHENM_LEN 0x02
#define E_PHESZ 0x36
#define E_PHESZ_LEN 0x02
#define E_ENTRY 0x18

#define PHOFF_TYPE 0x00
#define PHLEN_TYPE 0x04
#define PHOFF_FLAGS 0x04
#define PHLEN_FLAGS 0x04
#define PHOFF_FOFFS 0x08
#define PHLEN_FOFFS 0x08
#define PHOFF_PADDR 0x10
#define PHLEN_PADDR 0x08
#define PHOFF_VADDR 0x18
#define PHLEN_VADDR 0x08
#define PHOFF_FSIZE 0x20
#define PHLEN_FSIZE 0x08
#define PHOFF_MSIZE 0x28
#define PHLEN_MSIZE 0x08
#define PHOFF_ALIGN 0x30
#define PHLEN_ALIGN 0x08

#define PHTYP_NULL 0x0
#define PHTYP_LOAD 0x1
#define PHTYP_DYN  0x2
#define PHTYP_INRP 0x3
#define PHTYP_NOTE 0x4
#define PHTYP_MISC 0x5
#define PHTYP_PHDR 0x6
#define PHTYP_TLS  0x7

//0x60000000 - 6FFFFFFF OS_EXTENSIONS
//0x70000000 - 7FFFFFFF PROCESSOR_EXTENSIONS

#define PHFLG_EXEC 0x1
#define PHFLG_WRTE 0x2
#define PHFLG_READ 0x4


void PaxErrorReportELFread(char *section) {
	printf("%s:Failed to read elf section %s\n", PAX_ERROR, section);
	}

/*
*
* Fill Binary struct passed with all header information in e_hdr
* expaneds the e_ident into their subcatagories.
*
*/

PAX_RETURN_STATUS PaxParseElfHdr(Binary *bin) {
	lseek(bin->fd, E_IDENT, SEEK_SET);
	//				ELF READ e_magic
	if (read(bin->fd, bin->buff, 4) ==4 && memcmp(bin->buff, "\x7f\x45\x4c\x46", 4) == 0) {
		printf("%s: Valid ELF header found.\n",PAX_SUCCESS);
	}
	else {
		printf("%s: Not a valid ELF header. exiting.\n", PAX_ERROR);
		close(bin->fd);
		free(bin->buff);
		free(bin);
		return NOT_VALID_ELF;
	}
	lseek(bin->fd, E_CLASS, SEEK_SET);
	unsigned char width = 0xff;
	//				ELF_READ e_class
	if (read(bin->fd, &width, 1) ==1) {
		printf("[SUCCESS]: Found max width of %s-bit\n", (width == 0x02)?"64":"32");
	}
	if (width == 0x01) { //32-bit 
		printf("32-bit not supported.");
		//read_ph32();
	}
	else if (width == 0x02) {
		// ELF_READ REST OF ELF
		bin->read = read(bin->fd,&bin->e_hdr.Data, ELF_DATA_LEN);
		if (bin->read <= 0) {PaxErrorReportELFread("Data");}
		bin->read = read(bin->fd,&bin->e_hdr.eVersion, ELF_EVERSION_LEN);
		if (bin->read <= 0) {PaxErrorReportELFread("eVersion");}
		bin->read = read(bin->fd,&bin->e_hdr.ABI, ELF_ABI_LEN);
		if (bin->read <= 0) {PaxErrorReportELFread("ABI");}
		bin->read = read(bin->fd,&bin->e_hdr.Padding, ELF_PADDING_LEN);
		if (bin->read <= 0) {PaxErrorReportELFread("Padding");}
		bin->read = read(bin->fd,&bin->e_hdr.Type, ELF_TYPE_LEN);
		if (bin->read <= 0) {PaxErrorReportELFread("Type");}
		bin->read = read(bin->fd,&bin->e_hdr.Arch, ELF_ARCH_LEN);
		if (bin->read <= 0) {PaxErrorReportELFread("Arch");}
		bin->read = read(bin->fd,&bin->e_hdr.Version, ELF_VERSION_LEN);
		if (bin->read <= 0) {PaxErrorReportELFread("Version");}
		bin->read = read(bin->fd,&bin->e_hdr.Entry, ELF_ENTRY_LEN);
		if (bin->read <= 0) {PaxErrorReportELFread("Entry");}
		bin->read = read(bin->fd,&bin->e_hdr.PHEntry, ELF_PHENTRY_LEN);
		if (bin->read <= 0) {PaxErrorReportELFread("PHEntry");}
		bin->read = read(bin->fd,&bin->e_hdr.SHEntry, ELF_SHENTRY_LEN);
		if (bin->read <= 0) {PaxErrorReportELFread("SHEntry");}
		bin->read = read(bin->fd,&bin->e_hdr.Flags, ELF_FLAGS_LEN);
		if (bin->read <= 0) {PaxErrorReportELFread("Flags");}
		bin->read = read(bin->fd,&bin->e_hdr.ESize, ELF_ESIZE_LEN);
		if (bin->read <= 0) {PaxErrorReportELFread("ESize");}
		bin->read = read(bin->fd,&bin->e_hdr.PHESize, ELF_PHESIZE_LEN);
		if (bin->read <= 0) {PaxErrorReportELFread("PHESize");}
		bin->read = read(bin->fd,&bin->e_hdr.PHENum, ELF_PHENUM_LEN);
		if (bin->read <= 0) {PaxErrorReportELFread("PHENum");}
		bin->read = read(bin->fd,&bin->e_hdr.SHENSize, ELF_SHENSIZE_LEN);
		if (bin->read <= 0) {PaxErrorReportELFread("SHENSize");}
		bin->read = read(bin->fd,&bin->e_hdr.SHENum, ELF_SHENUM_LEN);
		if (bin->read <= 0) {PaxErrorReportELFread("SHENum");}
		bin->read = read(bin->fd,&bin->e_hdr.SHSTRNDX, ELF_SHSTRNDX_LEN);
		if (bin->read <= 0) {PaxErrorReportELFread("SHSTRNDX");}
		}
		return PAX_OKAY;

	}


PAX_RETURN_STATUS read_ph(Binary *b) {
	b->phtable = malloc(sizeof(PHENTRY)* b->e_hdr.PHENum);
	lseek(b->fd, b->e_hdr.PHEntry, SEEK_SET);
	for (int i = 0; i < b->e_hdr.PHENum; ++i) {
		b->read= read(b->fd, b->buff, b->e_hdr.PHESize);
		printf("Entry: \t%d/n\n", i);
		memcpy(&b->phtable[i].type, &b->buff[PHOFF_TYPE], PHLEN_TYPE);
		printf("Type: %lx\n",b->phtable[i].type);
		memcpy(&b->phtable[i].flags, &b->buff[PHOFF_FLAGS], PHLEN_FLAGS);
		if (b->phtable[i].flags & PHFLG_EXEC) {
		b->xseg = i;
		}
		printf("Flags: \t%lx\n",b->phtable[i].flags);
		memcpy(&b->phtable[i].offset, &b->buff[PHOFF_FOFFS], PHLEN_FOFFS);
		printf("File Offset: \t0x%lx\n",b->phtable[i].offset);
		memcpy(&b->phtable[i].vaddr, &b->buff[PHOFF_VADDR], PHLEN_VADDR);
		printf("Virtual Address: \t0x%lx\n",b->phtable[i].vaddr);
		memcpy(&b->phtable[i].paddr, &b->buff[PHOFF_PADDR], PHLEN_PADDR);
		printf("Physical Address: \t0x%lx\n",b->phtable[i].paddr);
		memcpy(&b->phtable[i].filesz, &b->buff[PHOFF_FSIZE], PHLEN_FSIZE);
		printf("File Size: \t%lx\n",b->phtable[i].filesz);
		memcpy(&b->phtable[i].memsz, &b->buff[PHOFF_MSIZE], PHLEN_MSIZE);
		printf("Memory Size: \t%lx\n",b->phtable[i].memsz);
		memcpy(&b->phtable[i].align, &b->buff[PHOFF_ALIGN], PHLEN_ALIGN);
		printf("Alignment: \t%lx\n",b->phtable[i].align);
		
		}
		return PAX_OKAY;
	}
