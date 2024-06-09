//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//           INSTITUTO POLITECNICO DO CAVADO E DO AVE
//                          2022/2023
//             ENGENHARIA DE SISTEMAS INFORMATICOS
//                    VISAO POR COMPUTADOR
//
//             [  DUARTE DUQUE - dduque@ipca.pt  ]
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// #define VC_DEBUG

#ifndef VC_H
#define VC_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

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
	int channels;			// Binario/Cinzentos=1; RGB=3
	int levels;				// Binario=1; Cinzentos [1,255]; RGB [1,255]
	int bytesperline;		// width * channels
} IVC;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    PROTOTIPOS DE FUNCOES
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// DESENHAR
int mostrar_zona_analise(IVC *image, float lateral_cutoff, float header_cutoff, float footer_cutoff);

int binarizar_1ch_8bpp(IVC *image, IVC *sem_fundo_bin, int int_fundo);
int draw_box(OVC *array_blobs_relevantes, IVC *image, int count_relevantes);

// BINARIZAR

int apagar_fora_de_zona(IVC *sem_fundo_bin, float lateral_cutoff, float header_cutoff, float footer_cutoff );

// BLOBS
int vc_binary_blob_info(IVC *src, OVC *blobs, int nblobs);
OVC* vc_binary_blob_labelling(IVC *src, IVC *dst, int *nlabels);
OVC* filter_blobs (OVC *array_blobs, int nlabels, int *count_relevantes, int area_min, int area_max, int altura_min, int altura_max, int largura_min, int largura_max);
int ajustar_blobs(OVC *array_blobs_relevantes, int count_relevantes);

// RESISTENCIAS
int copiar_frame_nova_imagem(IVC *image, IVC *frame, OVC *blob  );
	int retirar_blob_RGB(IVC *image,IVC *sample,OVC *current_blob);
	int vc_rgb_to_hsv (IVC *sample);
int analisar_resistencias (OVC *array_blobs_relevantes, int count_relevantes, IVC *image, int *cores_memoria);

// CORES
int cor_seccao(IVC *sample, int *cores_memoria);
int cor_identificar_banda1(int hue, int sat, int val);
int cor_identificar_banda2(int hue, int sat, int val);
int cor_identificar_banda3(int hue, int sat, int val);
bool analisar_cores_memoria(int *cores_memoria, int *contar_220, int *contar_1000, int *contar_2200, int *contar_5600, int *contar_10000);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// FUNCOES: ALOCAR E LIBERTAR UMA IMAGEM
IVC *vc_image_new(int width, int height, int channels, int levels);
IVC *vc_image_free(IVC *image);

// FUNCOES: LEITURA E ESCRITA DE IMAGENS (PBM, PGM E PPM)
IVC *vc_read_image(char *filename);
int vc_write_image(char *filename, IVC *image);

// OUTRAS FUNÇOES DE SUPORTE
void bit_to_unsigned_char(unsigned char *databit, unsigned char *datauchar, int width, int height);
long int unsigned_char_to_bit(unsigned char *datauchar, unsigned char *databit, int width, int height);
char *netpbm_get_token(FILE *file, char *tok, int len);

#ifdef __cplusplus
}
#endif

#endif