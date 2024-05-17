//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//           INSTITUTO POLIT�CNICO DO C�VADO E DO AVE
//                          2022/2023
//             ENGENHARIA DE SISTEMAS INFORM�TICOS
//                    VIS�O POR COMPUTADOR
//
//             [  DUARTE DUQUE - dduque@ipca.pt  ]
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// #define VC_DEBUG

#ifndef VC_H
#define VC_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                   ESTRUTURA DE UMA IMAGEM
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

typedef struct {
	int x, y, width, height;	// Caixa Delimitadora (Bounding Box)
	int area;					// Área
	int xc, yc;					// Centro-de-massa
	int perimeter;				// Perímetro
	int label;					// Etiqueta
} OVC;

typedef struct {
	unsigned char *data;
	int width, height;
	int channels;			// Bin�rio/Cinzentos=1; RGB=3
	int levels;				// Bin�rio=1; Cinzentos [1,255]; RGB [1,255]
	int bytesperline;		// width * channels
} IVC;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    PROT�TIPOS DE FUN��ES
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// FUN��ES: ALOCAR E LIBERTAR UMA IMAGEM
IVC *vc_image_new(int width, int height, int channels, int levels);
IVC *vc_image_free(IVC *image);

// FUN��ES: LEITURA E ESCRITA DE IMAGENS (PBM, PGM E PPM)
IVC *vc_read_image(char *filename);
int vc_write_image(char *filename, IVC *image);

// OUTRAS FUNÇOES DE SUPORTE
void bit_to_unsigned_char(unsigned char *databit, unsigned char *datauchar, int width, int height);
long int unsigned_char_to_bit(unsigned char *datauchar, unsigned char *databit, int width, int height);
char *netpbm_get_token(FILE *file, char *tok, int len);

// FUNÇOES CONVERSAO
int vc_rgb_get_red_gray(IVC *srcdst);
int vc_rgb_to_gray(IVC *src, IVC *dst);
int vc_rgb_to_hsv(IVC *srcdst);

// SEGMENTACAO
int vc_hsv_segmentation(IVC *srcdst, int hmin, int hmax, int smin, int smax, int vmin, int vmax);
int vc_hsv_segmentation_output(IVC *PETNormal, IVC *VC_gray, int, int, int, int, int, int);
int countWhitePixels(IVC *srcdst);
int vc_gray_to_binary_midpoint(IVC *src, IVC *dst, int kernel);
int vc_scale_gray_to_rgb(IVC *src, IVC *dst);
int vc_gray_to_binary(IVC *srcdst, IVC *dst, int threshold);
float vc_gray_to_binary_global_mean(IVC *srcdst);
int vc_gray_to_binary_midpoint(IVC *src, IVC *dst, int kernel);
int vc_gray_to_binary_Bernsen(IVC *src, IVC *dst, int kernel);
int vc_gray_to_binary_niblack(IVC* src, IVC* dst, int kernel, float k);

// OPERADORES MORFOLOGICOS
int vc_binary_dilate(IVC* src, IVC* dst, int kernel);
int vc_binary_erode(IVC* src, IVC* dst, int kernel);
int vc_binary_open(IVC *src, IVC *dst, int kernelE, int kernelD);
int vc_binary_close(IVC *src, IVC *dst, int kernelE, int kernelD);

int vc_gray_dilate(IVC *src, IVC *dst, int kernel);
int vc_gray_erode(IVC *src, IVC *dst, int kernel);
int vc_gray_close(IVC *src, IVC *dst, int kernelE, int kernelD);
int vc_gray_open(IVC *src, IVC *dst, int kernelE, int kernelD);


// ETIQUETAGEM

int vc_binary_blob_info(IVC *src, OVC *blobs, int nblobs);
OVC* vc_binary_blob_labelling(IVC *src, IVC *dst, int *nlabels);
int vc_labeled_to_grey(IVC* imageA_labelled, IVC* imageA_label_grey, int nlabels, int width, int height);

// HISTOGRAM

int vc_gray_histogram_show(IVC *src, IVC *dst);
int vc_gray_histogram_equalization(IVC *src, IVC *dst);

// DETEÇÃO DE CONTORNOS
int vc_grey_edge_prewitt(IVC *image_original, IVC *barbara_contornos, float th);

#endif