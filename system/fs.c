//Abhishek Naik
#include <xinu.h>
#include <kernel.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#if FS
#include <fs.h>

static struct fsystem fsd;
int dev0_numblocks;
int dev0_blocksize;
char *dev0_blocks;

extern int dev0;

char block_cache[512];

#define SB_BLK 0
#define BM_BLK 1
#define RT_BLK 2
#define NUM_FD 16
struct filetable oft[NUM_FD];
int next_open_fd = 0;

#define INODES_PER_BLOCK (fsd.blocksz / sizeof(struct inode))
#define NUM_INODE_BLOCKS (( (fsd.ninodes % INODES_PER_BLOCK) == 0) ? fsd.ninodes / INODES_PER_BLOCK : (fsd.ninodes / INODES_PER_BLOCK) + 1)
#define FIRST_INODE_BLOCK 2

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock);

/* YOUR CODE GOES HERE */
int fs_fileblock_to_diskblock(int dev, int fd, int fileblock) {
  int diskblock;

  if (fileblock >= INODEBLOCKS - 2) {
    printf("No indirect block support\n");
    return SYSERR;
  }

  
  diskblock = oft[fd].in.blocks[fileblock]; //get the logical block address

  return diskblock;
}

/* read in an inode and fill in the pointer */
int 
fs_get_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;
  int inode_off;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_get_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  inode_off = inn * sizeof(struct inode);

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  printf("inn*sizeof(struct inode): %d\n", inode_off);
  */
  bs_bread(dev0, bl, 0, &block_cache[0], fsd.blocksz);
  memcpy(in, &block_cache[inode_off], sizeof(struct inode));

  return OK;

}

int 
fs_put_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_put_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  */
  bs_bread(dev0, bl, 0, block_cache, fsd.blocksz);
  memcpy(&block_cache[(inn*sizeof(struct inode))], in, sizeof(struct inode));
  bs_bwrite(dev0, bl, 0, block_cache, fsd.blocksz);

  return OK;
}
     
int fs_mkfs(int dev, int num_inodes) {
  int i;
  
  if (dev == 0) {
    fsd.nblocks = dev0_numblocks;
    fsd.blocksz = dev0_blocksize;
  }
  else {
    printf("Unsupported device\n");
    return SYSERR;
  }

  if (num_inodes < 1) {
    fsd.ninodes = DEFAULT_NUM_INODES;
  }
  else {
    fsd.ninodes = num_inodes;
  }

  i = fsd.nblocks;
  while ( (i % 8) != 0) {i++;}
  fsd.freemaskbytes = i / 8; 
  
  if ((fsd.freemask = getmem(fsd.freemaskbytes)) == (void *)SYSERR) {
    printf("fs_mkfs memget failed.\n");
    return SYSERR;
  }
  
  /* zero the free mask */
  for(i=0;i<fsd.freemaskbytes;i++) {
    fsd.freemask[i] = '\0';
  }
  
  fsd.inodes_used = 0;
  
  /* write the fsystem block to SB_BLK, mark block used */
  fs_setmaskbit(SB_BLK);
  bs_bwrite(dev0, SB_BLK, 0, &fsd, sizeof(struct fsystem));
  
  /* write the free block bitmask in BM_BLK, mark block used */
  fs_setmaskbit(BM_BLK);
  bs_bwrite(dev0, BM_BLK, 0, fsd.freemask, fsd.freemaskbytes);

  return 1;
}

void 
fs_print_fsd(void) {

  printf("fsd.ninodes: %d\n", fsd.ninodes);
  printf("sizeof(struct inode): %d\n", sizeof(struct inode));
  printf("INODES_PER_BLOCK: %d\n", INODES_PER_BLOCK);
  printf("NUM_INODE_BLOCKS: %d\n", NUM_INODE_BLOCKS);
}

/* specify the block number to be set in the mask */
int fs_setmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  fsd.freemask[mbyte] |= (0x80 >> mbit);
  return OK;
}

/* specify the block number to be read in the mask */
int fs_getmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  return( ( (fsd.freemask[mbyte] << mbit) & 0x80 ) >> 7);
  return OK;

}

/* specify the block number to be unset in the mask */
int fs_clearmaskbit(int b) {
  int mbyte, mbit, invb;
  mbyte = b / 8;
  mbit = b % 8;

  invb = ~(0x80 >> mbit);
  invb &= 0xFF;

  fsd.freemask[mbyte] &= invb;
  return OK;
}

/* This is maybe a little overcomplicated since the lowest-numbered
   block is indicated in the high-order bit.  Shift the byte by j
   positions to make the match in bit7 (the 8th bit) and then shift
   that value 7 times to the low-order bit to print.  Yes, it could be
   the other way...  */
void fs_printfreemask(void) {
  int i,j;

  for (i=0; i < fsd.freemaskbytes; i++) {
    for (j=0; j < 8; j++) {
      printf("%d", ((fsd.freemask[i] << j) & 0x80) >> 7);
    }
    if ( (i % 8) == 7) {
      printf("\n");
    }
  }
  printf("\n");
}

int fs_create(char *filename, int fs_mode)
{
	int flength, fs_status;
	int i, fs_oftindex;
	struct inode in;
		
	/* Checking the file mode*/
	if(fs_mode != O_CREAT)
	{
		printf("\nPlease enter the correct fs_mode");
		printf("\nFile creation failed");
		return -1;
	}

	/*Checking if the file already exists*/
	for(i=0; i<fsd.root_dir.numentries; i++)
	{
		if(strcmp(fsd.root_dir.entry[i].name, filename)==0)
		{
			printf("\nFile already exists.  Please create another file by using another file name");
			return -1;
		}
	}
	
	/* Checking the length of the filename*/
	flength = strnlen(filename, FILENAMELEN);
	if(flength > FILENAMELEN)
	{
		printf("\nName of the file should be less than 32 characters");
		return -1;
	}
	
	/*check if any inode is available*/
	if(fsd.inodes_used >= fsd.ninodes)
	{
		printf("\nNo iNode available.");
		return -1;
	}
	
	fs_status = fs_get_inode_by_num(0, i, &in);
	if(fs_status == SYSERR)
	{
		printf("\nFailed in the function fs_get_inode_by_num() while creating the file");
		return -1;
	}
	
	/* getting inodes block by block*/
	for(i = 0; i < fsd.ninodes; i++)
		if(inodes_state[i] != USED)
			break;

	/* filling up the inode information*/	
	in.id = i;
	in.flags = INODE_TYPE_FILE;
	in.nlink = 0;
	in.device = 0;
	in.size = 0;

	/* writing the block back to the memory*/
	fs_status = fs_put_inode_by_num(0, i, &in);
	if(fs_status==SYSERR)
	{
		printf("\nFailed in the function fs_put_inode_by_num() while creating the file.");
		return -1;
	}

	/*marking inode as used in array*/
	inodes_state[i] = USED;
	
	fsd.root_dir.entry[fsd.root_dir.numentries].inode_num = i;
	strcpy(fsd.root_dir.entry[fsd.root_dir.numentries].name, filename);
		
	fsd.inodes_used++;
	fsd.root_dir.numentries++;
	fs_oftindex = fs_open(filename, O_RDWR);
	return fs_oftindex;
}

int fs_open(char *filename, int fs_flag)
{
	int i, j, fs_status;
	int fs_oftindex = -1;
	struct inode in;
	
	if(!(fs_flag == O_WRONLY || fs_flag == O_RDONLY || fs_flag == O_RDWR))
	{
		printf("\nPlease enter the correct file mode");
		return -1;
	}
	
	/*check if file is present*/
	for(i=0; i<fsd.root_dir.numentries; i++)
		if(strcmp(fsd.root_dir.entry[i].name, filename)==0)

	if(i >= fsd.root_dir.numentries)
	{
		printf("\nFile is not present");
		return -1;	
	}
	
	/*check if file is already opened */
	for(j=0; j<NUM_FD; j++)
	{
		if(oft[j].in.id == fsd.root_dir.entry[i].inode_num)
		{
			if(oft[j].state == FSTATE_CLOSED)
			{
				fs_oftindex = j;
				break;
			}
			else if(oft[j].state == FSTATE_OPEN)
			{
				printf("\nThe file is already open");
				return -1;
			}
		}
		if(oft[j].state == FSTATE_CLOSED && fs_oftindex == -1)
			fs_oftindex = j;
	}
	
	if(fs_oftindex == -1)
	{
		printf("\nOpen file table is full");
		return -1;
	}
	
	/* retrieve the inode block by the inode number */
	fs_status = fs_get_inode_by_num(0, fsd.root_dir.entry[i].inode_num, &in);
	if(fs_status == SYSERR)
	{
		printf("\nError in get_inode_by_num in opening file");
		return -1;
	}
	
	/*openfiletable entry */
	oft[fs_oftindex].state = FSTATE_OPEN;
	oft[fs_oftindex].fileptr = 0;
	oft[fs_oftindex].de = &fsd.root_dir.entry[i];
	oft[fs_oftindex].in = in;
	oft[fs_oftindex].mode = fs_flag;
	//printf("\noft[fs_oftindex].mode %d",oft[fs_oftindex].mode);
	return fs_oftindex;
}

int min(int a, int b)
{
	if(a > b) 
		return b;
	return a;
}

int writeBlock(int fs_dev, int fs_block, int fs_offset, void *buf, int fs_len)
{
	return bs_bwrite(fs_dev, fs_block, fs_offset, buf, fs_len);
}

int fs_write(int fs_fd, void *buf, int fs_nbytes)
{
	int i, j, fs_status;
	int blocksToWrite=0;
	struct inode fs_temp_Inode;
	int fs_bytesToWrt=0, mifs_nbytes=0;

	/*Checking the file descriptor*/
	if(fs_fd<0 || fs_fd>NUM_FD)
	{
		printf("\nInvalid file descriptor");
		return -1;
	}

	/* Checking for valid fs_nbytes*/
	if(fs_nbytes <=0 || strlen((char*)buf)==0)
	{
		printf("\nInvalid length of write buffer");
		return -1;	
	}
	
	if(oft[fs_fd].state != FSTATE_OPEN || (oft[fs_fd].mode != O_RDWR && oft[fs_fd].mode != O_WRONLY))
	{
		printf("\nCannot write to file in read mode");
		return -1;
	}
	
	/*Checking if data is already present in file ...overwrite if present*/
	if(oft[fs_fd].in.size > 0)
	{
		fs_temp_Inode = oft[fs_fd].in;
		for(;oft[fs_fd].in.size>0;)
		{
			if(fs_clearmaskbit(fs_temp_Inode.blocks[oft[fs_fd].in.size-1]) != OK)
			{
				printf("\n Error in clearing block %d..in write",oft[fs_fd].in.size-1);
				return -1;
			}
			oft[fs_fd].in.size--;
		}
	}
	
	if(strlen((char*)buf) < fs_nbytes)
		fs_nbytes = strlen((char*)buf);
	
	blocksToWrite = fs_nbytes / MDEV_BLOCK_SIZE;
	if(fs_nbytes % MDEV_BLOCK_SIZE !=0){
		blocksToWrite++;
	}
	
	fs_bytesToWrt = fs_nbytes;
	
	/*Finding the block to write the data*/
	j = FIRST_INODE_BLOCK + NUM_INODE_BLOCKS; 
	for(i=0; i<blocksToWrite && j<MDEV_BLOCK_SIZE; j++)
	{
		if(fs_getmaskbit(j) == 0)
		{
			/*Clearing the data block to write*/
			memset(block_cache, NULL, MDEV_BLOCK_SIZE);
			if(writeBlock(0, j, 0, block_cache, MDEV_BLOCK_SIZE) == SYSERR)
			{
				printf("\nUnable to free block %d",j);
				return -1;
			}
			mifs_nbytes = min(MDEV_BLOCK_SIZE, fs_bytesToWrt);
			memcpy(block_cache, buf, mifs_nbytes);
			
			/*Writing the data to the data block */
			if(writeBlock(0, j, 0, block_cache, MDEV_BLOCK_SIZE) == SYSERR)
			{
				printf("\nUnable to write to block %d",j);
				return -1;
			}
			
			buf = (char*) buf + mifs_nbytes;
			fs_bytesToWrt = fs_bytesToWrt - mifs_nbytes;
			fs_setmaskbit(j);
			
			oft[fs_fd].in.blocks[i++] = j;
		}
	}
	oft[fs_fd].in.size = blocksToWrite;

	fs_status = fs_put_inode_by_num(0, oft[fs_fd].in.id, &oft[fs_fd].in);
	if(fs_status==SYSERR)
	{
		printf("\nError in the function put_inode_by_number() in write ");
		return -1;
	}
	oft[fs_fd].filesize = fs_nbytes;
	oft[fs_fd].fileptr = fs_nbytes;
	return fs_nbytes;
}

int readBlock(int fs_dev, int fs_blck, int fs_offset, void *buf, int fs_len)
{
	return bs_bread(fs_dev, fs_blck, fs_offset, buf, fs_len);
}

int fs_read(int fs_fd, void *buf, int fs_bytes)
{
	int i, j, status, tmp=0;
	int blocksToRead=0;
	struct inode tmpInode;
	int fs_bytesRd=0, bytes=0;
	
	/* Checking if fs_fd is valid*/
	if(fs_fd<0 || fs_fd>NUM_FD)
	{
		printf("\nInvalid file descriptor.  Please enter valid one");
		return -1;
	}

	if(oft[fs_fd].state != FSTATE_OPEN || (oft[fs_fd].mode != O_RDWR && oft[fs_fd].mode != O_RDONLY))
	{
		printf("\nOpening the file in read/read-write mode");
		return -1;
	}
	
	if(fs_bytes <=0)
	{
		printf("\nEnter valid bytes to read");
		return -1;	
	}
	
	if(oft[fs_fd].in.size == 0)
	{
		printf("\nThe file is empty");
		return 0;
	}
	fs_bytes += oft[fs_fd].fileptr;
	blocksToRead = fs_bytes / MDEV_BLOCK_SIZE;

	if(fs_bytes % MDEV_BLOCK_SIZE !=0)
		blocksToRead++;
	
	blocksToRead = min(oft[fs_fd].in.size, blocksToRead);
	i = (oft[fs_fd].fileptr / MDEV_BLOCK_SIZE);

	memset(buf, NULL, MAXSIZE);

	for(j=(oft[fs_fd].fileptr % MDEV_BLOCK_SIZE); i<blocksToRead; i++)
	{
		memset(block_cache, NULL, MDEV_BLOCK_SIZE+1);
		
		if(readBlock(0, oft[fs_fd].in.blocks[i], j, block_cache, MDEV_BLOCK_SIZE-j) == SYSERR)
		{
			printf("\nError in reading the file");
			return -1;
		}
		strcpy((buf+tmp), block_cache);

		tmp = strlen(buf);

		j=0;

	}
	oft[fs_fd].fileptr = tmp;
	return tmp;
}

int fs_seek(int fs_fd, int offset)
{
	int position=1;
	
	/* Checking if fs_fd id valid*/
	if(fs_fd<0 || fs_fd>NUM_FD)
	{
		printf("\n Invalid file descriptor.");
		return -1;
	}
	/* Checking if the file is already open.*/
	if(oft[fs_fd].state != FSTATE_OPEN)
	{
		printf("\nFile should be open in order to seek");
		return -1;
	}

	switch(position)
	{
		case FS_BEGIN: oft[fs_fd].fileptr = 0; break;
		
		case FS_CURR: break;

		case FS_EOF: oft[fs_fd].fileptr = oft[fs_fd].filesize; break;

		default: printf("\nPlease enter the correct position"); return -1;
	}
	
	if((offset + oft[fs_fd].fileptr) < 0)
	{
		printf("\nOffset should be non-negative");
		return -1;
	}
	
	if(oft[fs_fd].filesize < (offset + oft[fs_fd].fileptr))
		oft[fs_fd].fileptr = oft[fs_fd].filesize;
	else
		oft[fs_fd].fileptr = oft[fs_fd].fileptr + offset;

	return oft[fs_fd].fileptr;
}

int fs_close(int fs_fd)
{
	if(fs_fd < 0 || fs_fd >= NUM_FD)
	{
		printf("\nInvalid file descriptor");
		return -1;
	}

	if(oft[fs_fd].state == FSTATE_OPEN)
	{
		oft[fs_fd].state = FSTATE_CLOSED;
		oft[fs_fd].fileptr = 0;
		return OK;
	}
	return -1;
}
#endif /* FS */