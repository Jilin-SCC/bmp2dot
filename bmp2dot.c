#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

int getBmpWidth(FILE* fpbmp);
int getBmpHeight( FILE* fpbmp );
int getBmpFileSize(FILE * fpbmp);
int getBmpPixBits(FILE * fpbmp); 
int getOffset(FILE * fpbmp);
int readBmpData( FILE* fpbmp );
int readBmpData_1bit( FILE* fpbmp, unsigned char *code_temp );
int translate(unsigned char *code_temp, char *out_file_name, FILE* fpbmp);
void print_para_error();
char inputfilename[20];
char outputfilename[20];
unsigned char *code_temp;
int main(int argc, char const *argv[])
{
	if (argc < 2)
	{
		print_para_error();
		printf("	\nPress ENTER To Exit!\n");
		getchar();
		return 0;
	}else if (argc == 2)
	{
		strcpy(inputfilename,argv[1]);
		strcpy(outputfilename,argv[1]);
		strcat(outputfilename,".out");
	}else if (argc == 3)
	{
		strcpy(inputfilename,argv[1]);
		strcpy(outputfilename,argv[2]);
	}else
	{
		print_para_error();
		printf("	\nPress ENTER To Exit!\n");
		getchar();
		return 0;
	}

    FILE * fp_read ;    
    fp_read = fopen(inputfilename,"rb"); 
    if(fp_read == NULL)
    {
        printf("Failed to open file:%s\r\n",inputfilename);
        return 0;
    }
    
    printf("Image  Width: %d\n",getBmpWidth(fp_read));
    printf("Image Heigth: %d\n",getBmpHeight(fp_read));
    printf("Image   Size: %d Byte\n",getBmpFileSize(fp_read));
    printf("Bits per Pix: %d\n",getBmpPixBits(fp_read));
    printf("Data  Offset: %d\n",getOffset(fp_read));
    
    switch (getBmpPixBits(fp_read))
    {
        case 1:
            readBmpData_1bit(fp_read,code_temp);break;
        case 24:
            readBmpData(fp_read);break;
        default :
        break;
    }
    fclose(fp_read);
    fp_read = NULL;
    

    
    getchar();
    return 0;
}

int translate(unsigned char *code_temp, char *out_file_name, FILE *fpbmp)
{
	unsigned char data =0;
	unsigned char discode = 0;
    int width = getBmpWidth(fpbmp);
    int height = getBmpHeight(fpbmp);
	int one_line_byte = ceil((double)width/8);

	FILE* fp = NULL;
	fp = fopen(out_file_name,"w");
	if (fp == NULL)
	{
		printf("Failed to open file:%s\n\n 	Press ENTER To Exit!",out_file_name);
		getchar();
		return 0;
	}
	for (int i = 0; i < height; ++i)
	{
		printf("[%2x]", i);
		for (int j = 0; j < one_line_byte; ++j)
		{
			data = code_temp[i*one_line_byte+j];
			for (int k = 0; k < 8; ++k)
			{
				discode = (data << k) & 0x80;
				if (discode)
				{
					printf("/");
					fwrite("/",1,1,fp);
				}else{
					printf(" ");
					fwrite(" ",1,1,fp);
				}
			}
		}
        sleep(1);
		printf("\n");
		fprintf(fp,"\r\n");	
	}
	fclose(fp);
	fp = NULL;
	printf("Conversion complete! Bitmap data saved to %s .\n", outputfilename);
	return 1;
}


int readBmpData_1bit( FILE* fpbmp ,unsigned char *code_temp)
{
	FILE *fpw = fopen(outputfilename,"wb");
	if (fpw == NULL)
	{
		printf("Failed to open file:%s\n\n 	Press ENTER To Exit!",outputfilename);
		getchar();
		return -1;
	}
    int i = 0, j = 0;    
    int offset = getOffset(fpbmp);
    int width = getBmpWidth(fpbmp);
    int height = getBmpHeight(fpbmp);
    int bit_p_pix = getBmpPixBits(fpbmp);
    int one_line_byte = ceil((double)width/8);

    unsigned char *pix;
    pix = (unsigned char * )malloc( one_line_byte * sizeof( unsigned char ) );
    code_temp = (unsigned char *)calloc(sizeof(char),one_line_byte*height);
    fseek(fpbmp, offset, SEEK_SET); // Jump to data part

    for( i = 0; i < height; i++ )
    {
    	fread(pix, one_line_byte, 1, fpbmp);
        memcpy(code_temp + (height - 1 - i) * one_line_byte, pix, one_line_byte);
        if (one_line_byte%4)
        {
            for(int k = 0; k < 4 - one_line_byte%4; ++k)
            {
                fread(pix, 1, 1, fpbmp);
            }
        }
    }
    
    fwrite(code_temp,(width * height)/8,1,fpw);
    fclose(fpw);
    printf("Done! Binary data save in %s.\n", outputfilename);
    fpw = NULL;
    free(pix);

    memset(outputfilename,0,strlen(outputfilename));
    strcpy(outputfilename,inputfilename);
	strcat(outputfilename,".txt");
    translate(code_temp, outputfilename, fpbmp);

    free(code_temp);
    return 0;
}
void print_para_error()
{
	printf("The parameters are incorrect. Please try again.\n\n");
	printf("exe ----This application is used to convert 1-bit BMP\n"
	       "        images into bitmap data. You must specify the\n"
	       "        BMP file application to work properly. \n"
	       "        Of course, You can specify the output binary,\n"
	       "        and the bitmap data file will be automatically \n"
	       "        created and named [InputBmpFile].txt.\n\n\n");
	printf(".   exe [inputfile]  \n");
	printf("or\n");
	printf(".   exe [InputBmpFile] [OutputBinaryFile]\n");	
}
int readBmpData( FILE* fpbmp )
{
    int i = 0, j = 0;    
    int offset = getOffset(fpbmp);
    int width = getBmpWidth(fpbmp);
    int height = getBmpHeight(fpbmp);
    
    unsigned char * pix = NULL;

    //one pix have 3 byte data( R G B )
    pix = (unsigned char * )malloc( 4 * sizeof( unsigned char ) );

    fseek(fpbmp, offset, SEEK_SET); // Jump to data part

    for( i = 0; i < height; i++ )
    {
        for( j = 0; j < width; j++ )
        {
            fread(pix, 3, 1, fpbmp);
            printf("(%3d,%3d,%3d)  ",pix[0],pix[1],pix[2]);
        }
        
        printf(",");
        for(;(j)%4!=0;j++)
        {
            fread(pix, 3, 1, fpbmp);
            printf("(%3d,%3d,%3d) ",pix[0],pix[1],pix[2]);
        }
        printf("\n");
    }
    return 0;
}


//获取像素位数 
int getBmpPixBits(FILE * fpbmp)
{
    int bits = 0;
    
    fseek(fpbmp, 28, SEEK_SET);
    fread(&bits, sizeof(char), 2, fpbmp);
    
    return bits;
} 

int getBmpFileSize(FILE * fpbmp)
{
    int size = 0;
    
    fseek(fpbmp, 2, SEEK_SET);
    fread(&size, sizeof(char), 4, fpbmp);
    
    return size;
} 

int getBmpWidth(FILE* fpbmp)
{
    int width = 0;

    fseek(fpbmp, 18L, SEEK_SET);
    fread(&width, sizeof(char), 4, fpbmp);

    return width;
}

int getBmpHeight( FILE* fpbmp )
{
    int height = 0;

    fseek(fpbmp, 22L, SEEK_SET);
    fread(&height, sizeof(char), 4, fpbmp);

    return height;
}

int getOffset(FILE * fpbmp)
{
    int offset = 0;

    fseek(fpbmp, 10L, SEEK_SET);
    fread(&offset, sizeof(char), 4, fpbmp);

    return offset;
}
