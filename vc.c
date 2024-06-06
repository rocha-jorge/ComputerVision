//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//           INSTITUTO POLIT�CNICO DO C�VADO E DO AVE
//                          2022/2023
//             ENGENHARIA DE SISTEMAS INFORM�TICOS
//                    VIS�O POR COMPUTADOR
//
//             [  DUARTE DUQUE - dduque@ipca.pt  ]
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Desabilita (no MSVC++) warnings de fun��es n�o seguras (fopen, sscanf, etc...)
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "vc.h"

#include <stdbool.h>

#define MY_MAX(a, b ) (a > b ? a : b)


bool analisar_cores_memoria(cores_memoria){

	// analisar sequencia de cores
	return false;

		// resistencias no video
	// 1. Verde 	Azul 		Vermelho 	Dourado   	5 6 *100	 5600
	// 2. Vermelho 	Vermelho 	Castanho 	Dourado		2 2 *10		  220
	// 3. Castanho	Preto		Vermelho	Dourado		1 0 *100	 1000
	// 4. Vermelho	Vermelho	Vermelho	Dourado		2 2 *100	 2200
	// 5. Castanho	Preto		Laranja		Dourado		1 0 *1000	10000
	// 6. Castanho	Preto		Vermelho	Dourado		1 0 *100	 1000

	// Dourado  = "Os grupos podem considerar que todas as resistências possuem uma tolerância de ±5%" pelo que não é necessário avaliar

}


int analisar_blobs (OVC *array_blobs_relevantes, int count_relevantes, IVC *image, int *cores_memoria){

	for (int i = 0; i<count_relevantes; i++){	// para cada blob
		OVC *current_blob = &array_blobs_relevantes[i];

		// definir tamanho da sample a tirar do blob
		int sample_width = current_blob->width;
		int sample_height = 4;

		// retira a sample do blob e converte BGR->RGB
 		IVC *sample = vc_image_new(sample_width, sample_height, 3, image->levels);
		retirar_blob_RGB(image,sample,current_blob);
			//vc_write_image("sampleRGB.ppm", sample);

		// converte sample para HSV
		vc_rgb_to_hsv(sample);  // hue toma o valor de 0 a 255 e não de 0 a 360
			//vc_write_image("sampleHSV.ppm", sample);

		// identificar a cor das 3 posições e guardar na memoria de cores
		cor_seccao(sample, cores_memoria);

	}
	return 1;
}

int cor_seccao(IVC *sample, int *cores_memoria){

	int banda1_ini = 0.12 * sample->width;
	int banda1_fin = 0.15 * sample->width;		// banda 1 a funcionar para todos
	int banda2_ini = 0.39 * sample->width;
	int banda2_fin = 0.41 * sample->width;
	int banda3_ini = 0.53 * sample->width;
	int banda3_fin = 0.58 * sample->width;

	int y_ini = 0;
	int y_fin = sample->height;


	// somatorios
	float hue_total, sat_total, val_total;
	hue_total = sat_total = val_total = 0 ;

	// secção
	int hue, sat, val;
	hue = sat = val = 0 ;
	int cor_id = 0;

	// indexes
	int pos = 0;
	int contar_pixeis = 0;

	// 1a banda / seccao ---------------------------------------
  	for( int y = 0 ; y < sample->height ; y++){
		for ( int x = banda1_ini*3 ; x < banda1_fin*3; x=x+3){
			pos = x + y * sample->bytesperline;
			hue_total += sample->data[pos] *360.0 / 255.0;
			sat_total += sample->data[pos+1];
			val_total += sample->data[pos+2];

			contar_pixeis++;
		}
	}

	hue = (int)hue_total / contar_pixeis;  // de 0 a 360
	sat = (int)sat_total / contar_pixeis;
	val = (int)val_total / contar_pixeis;

	int cor_1 = cor_identificar(hue, sat, val);
	if (cor_1 != 99999 ){	// se obtiver uma cor válida, guarda na memoria
			cores_memoria[0] = cor_1;
	}

/* 
	// 2a banda / seccao ---------------------------------------
	hue = sat = val = hue_total = sat_total = val_total = 0;  // reiniciar contagem para reutilizar variáveis
 	for( int y = 0 ; y < sample->height ; y++){
		for ( int x = banda2_ini*3 ; x < banda2_fin*3; x=x+3){
		
			pos = x + y * sample->bytesperline;
			hue_total += sample->data[pos] *360.0 / 255.0;
			sat_total += sample->data[pos+1] *360.0 / 255.0;
			val_total += sample->data[pos+2] *360.0 / 255.0;

			contar_pixeis++;
		}
	}

	hue = hue_total / contar_pixeis;
	sat = sat_total / contar_pixeis;
	val = val_total / contar_pixeis;

	int cor_2 = cor_identificar(hue, sat, val);
	if (cor_2 != 0 && cor_2 != 0 != 99999 ){
			cores_memoria[0] = cor_2;
	}
 */
	// 3a banda / seccao ---------------------------------------
/*  	hue = sat = val = hue_total = sat_total = val_total = 0;  // reiniciar contagem para reutilizar variáveis
 	for( int y = 0 ; y < sample->height ; y++){
		for ( int x = banda3_ini*3; x < banda3_fin*3; x=x+3){
		
			pos = x + y * sample->bytesperline;
			hue_total += sample->data[pos] *360.0 / 255.0;
			sat_total += sample->data[pos+1] *360.0 / 255.0;
			val_total += sample->data[pos+2] *360.0 / 255.0;

			contar_pixeis++;
		}
	}

	hue = (int)hue_total / contar_pixeis; // de 0 a 360
	sat = (int)sat_total / contar_pixeis;
	val = (int)val_total / contar_pixeis;

	int cor_3 = cor_identificar(hue, sat, val);
	if (cor_3 != 0 && cor_3 != 0 != 99999 ){
			cores_memoria[0] = cor_3;
	}
 */
	return 1;
}

// nesta funçao os valores de hue já entram em 0-360
int cor_identificar(int hue, int sat, int val){

	int color = 0;

	// HUE	(0-360)				// SATURATION				// VALUE

	// green band 1 está feito
	int green_hue_min = 70, 	green_sat_min = 97 , 		green_val_min = 109;
	int green_hue_max = 111, 	green_sat_max = 129 , 		green_val_max = 184;

	// green band 1 e 2 está feito
	int blue_hue_min = 	184,		blue_sat_min = 0 , 			blue_val_min = 0;
	int blue_hue_max = 	205, 	blue_sat_max = 255 , 		blue_val_max = 255;

	// red band 1 e 2 está feito
	int red1_hue_min =	0,		red1_sat_min = 155 ,		red1_val_min = 178 ;
	int red1_hue_max = 	30, 	red1_sat_max = 167 , 		red1_val_max = 187;

	int red2_hue_min =	237,	red2_sat_min = 155,			red2_val_min = 178 ;
	int red2_hue_max = 	360, 	red2_sat_max = 167 , 		red2_val_max = 187;
 
	int brown_hue_min = 6, 		brown_sat_min = 0 , 		brown_val_min = 0;
	int brown_hue_max = 19, 	brown_sat_max = 255 , 		brown_val_max = 255;

	int orange_hue_min = 31, 	orange_sat_min = 95 ,		orange_val_min = 0;
	int orange_hue_max = 45, 	orange_sat_max = 255 , 		orange_val_max = 255;

	int black_hue_min = 0,		black_sat_min = 0 , 		black_val_min = 0;
	int black_hue_max = 45, 	black_sat_max = 160 , 		black_val_max = 255; // isto distingue o preto
 
 	if (hue>black_hue_min && hue<black_hue_max && sat >black_sat_min && sat<black_sat_max && val<black_val_max){
		color = 1;
	}
	else if(hue>red1_hue_min && hue<red1_hue_max && sat >red1_sat_min && sat<red1_sat_max){
		color = 2;
	}
 	else if (hue>red2_hue_min && hue<red2_hue_max && sat >red2_sat_min && sat<red2_sat_max){
		color = 2;
	}
 	else if (hue>brown_hue_min && hue<brown_hue_max && sat >brown_sat_min && sat<brown_sat_max){
		color = 3;
	}
	else if (hue>orange_hue_min && hue<orange_hue_max && sat >orange_sat_min && sat<orange_sat_max){
		color = 4;
	}
	else if (hue>green_hue_min && hue<green_hue_max && sat >green_sat_min && sat<green_sat_max){
		color = 5;
 	}
    else if (hue>blue_hue_min && hue<blue_hue_max && sat >blue_sat_min && sat<blue_sat_max){
		color = 6;
	}
	else{
		color= 99999;
	}

	printf("\n \t Hue(0-360): %d   \t Sat: %d   \t Value: %d   \t Color: %d", hue , sat, val, color);

return color;
}

int retirar_blob_RGB(IVC *image, IVC *sample, OVC *blob  ){

	int bytesperline_image	= image->bytesperline;
	int bytesperline_sample = sample->bytesperline;
	int pos_image 			= 0;
	int pos_sample			= 0;

	for (int x = 0; x < sample->width; x++) {

		for( int y = 0; y < sample->height ; y++){

			pos_image = blob->x * 3 + x * 3     +  (blob->yc - sample->height/2 +  y) * bytesperline_image;
            pos_sample = x * 3 + y * bytesperline_sample;

 			sample->data[pos_sample]=image->data[pos_image+2];
			sample->data[pos_sample+1]=image->data[pos_image+1];
			sample->data[pos_sample+2]=image->data[pos_image];
		}
	}
return 1;
}

int copiar_frame_nova_imagem(IVC *image, IVC *frame, OVC *blob  ){

	int bytesperline_image	= image->bytesperline;
	int pos_image 			= 0;

	for (int x = 0; x < image->width*3; x=x+3) {

		for( int y = 0; y < image->height ; y++){

			pos_image = x + y * bytesperline_image;
			frame->data[pos_image]=image->data[pos_image+2];  // aqui é preciso trocar os channels para ficar RGB e abrir direito no GIMP
			frame->data[pos_image+1]=image->data[pos_image+1];
			frame->data[pos_image+2]=image->data[pos_image];  // aqui é preciso trocar os channels para ficar RGB e abrir direito no GIMP
		}
	}
return 1;
}

int ajustar_blobs(OVC *array_blobs_relevantes, int count_relevantes){
	
	// faz com que o comprimento do blob seja igual ao dobro (*.80) do comprimento do x inicial até ao centro de massa ()
	// na prática permite diminuir o blob retirando a zona que corresponde à sombra da resistência
	for (int i = 0; i<count_relevantes; i++){
		// retirar sombras do lado direito : lado direito do centro de massa é ajustado consoante o lado esquerdo
		array_blobs_relevantes[i].width = 2* 0.75* (array_blobs_relevantes[i].xc - array_blobs_relevantes[i].x);
			// atribuir largura máxima de uma resistência
			//int resist_width = 150;
			//array_blobs_relevantes[i].width = fmin(array_blobs_relevantes[i].width,resist_width);
		// como o blob ajustado se enquadra (quase) perfeitamente à resistência, faz sentido ajustar o centro de massa
		array_blobs_relevantes[i].xc = array_blobs_relevantes[i].x + array_blobs_relevantes[i].width/2;
	}
	return 1;
}

OVC *filter_blobs (OVC *array_blobs, int nlabels, int *count_relevantes, int area_min, int area_max, int altura_min, int altura_max, int largura_min, int largura_max){

	// criar array de tamanho nlabels (blobs relevantes <= numero de blobs)
	OVC *blobs_relevantes = (OVC *)calloc(nlabels, sizeof(OVC)); // as posição não ocupadas ficam a 0 ou null
		if (blobs_relevantes == NULL) {			// verificar se a alocação teve sucesso, caso contrário toda a lógica falha
		printf("Memory allocation failed\n");
		return NULL;
		}

	int blobs_ok = 0;

	for ( int i=0; i< nlabels ;i++){	// verificar se label está fora dos limites para ser relevante
		if (array_blobs[i].area < area_min || array_blobs[i].area > area_max || 
			array_blobs[i].height < altura_min || array_blobs[i].height > altura_max ||
			array_blobs[i].width < largura_min || array_blobs[i].width > largura_max) 
			{
			array_blobs[i].area = 0;
		}
		else{							// se estiver dentro dos limites -> array blobs_relevantes na 1a posição livre	
			blobs_relevantes[blobs_ok] = array_blobs[i];
			blobs_ok++;
		}
	}

	*count_relevantes = blobs_ok;

	return blobs_relevantes; // array
}

int draw_box(OVC * blobs_relevantes, IVC *image, int count_relevantes){

	int bytesperline = image->bytesperline;
	int height = image->height;
	int channels = image->channels;
	int pos = 0;

	for (int i = 0; i<count_relevantes; i++){ // para cada label
		
		OVC current_blob = blobs_relevantes[i];
			
		int x_ini = current_blob.x *3;
 		int x_fin = (current_blob.x + current_blob.width)*3;
		int y_ini = current_blob.y ;
		int y_fin = current_blob.y  + current_blob.height;

		// desenhar box
		for (int y=0; y<height; y++){
			for (int x=0; x<bytesperline; x=x+channels){
				if( ((y == y_ini || y == y_fin) && x >= x_ini && x <= x_fin ) ||
					((x == x_ini || x == x_fin) && y >= y_ini && y <= y_fin ) ) {				
					pos = x + bytesperline * y;
					image->data[pos] = 0;
					image->data[pos+1] = 0;
					image->data[pos+2] = 255;
				}
			}
		}

		// desenhar centro de massa
		pos = 0 ;
		int crosshair_x_ini = current_blob.xc*3 - 21; 	// bytes
		int crosshair_x_fin = current_blob.xc*3 + 21;; 	// bytes
		int crosshair_y_ini = current_blob.yc -  7;; 	// bytes
		int crosshair_y_fin = current_blob.yc +  7;; 	// bytes

		for (int y=0; y<height; y++){
			for (int x=0; x<bytesperline; x=x+channels){
				if ((x == current_blob.xc*3) && (y>=crosshair_y_ini) && (y<=crosshair_y_fin) ||
				    (y == current_blob.yc) && (x>=crosshair_x_ini) && (x<=crosshair_x_fin)){
						pos = x + bytesperline * y;
						image->data[pos] = 0;
						image->data[pos+1] = 0;
						image->data[pos+2] = 255;
				}
			}
		}
	}
	return 1;
}

int eliminar_cabos(IVC *sem_fundo_bin, int n_min_pixeis_vertical){

    int height = sem_fundo_bin->height;
    int bytesperline = sem_fundo_bin->bytesperline;

    int pixeis_vertical = 0;
    int pos = 0;

    for (int x = 0; x < bytesperline; x++) {
        pixeis_vertical = 0;
        for (int y = 0; y < height; y++) {
            pos = y * bytesperline + x;
            if (sem_fundo_bin->data[pos] == 255) {
                pixeis_vertical++;
            }
        }

        if (pixeis_vertical < n_min_pixeis_vertical) {
            for (int y = 0; y < height; y++) {
                pos = y * bytesperline + x;
                sem_fundo_bin->data[pos] = 0;
            }
        }
    }
	return 1;
}

int apagar_fora_de_zona(IVC *sem_fundo_bin, float lateral_cutoff, float header_cutoff, float footer_cutoff ){

	int height = sem_fundo_bin->height;
	int width = sem_fundo_bin->width;
	int bytesperline = sem_fundo_bin->bytesperline;

	int x_inicial = sem_fundo_bin->width  * lateral_cutoff;
	int x_final = sem_fundo_bin->width - x_inicial;
	int y_inicial = sem_fundo_bin->height * header_cutoff;
	int y_final = sem_fundo_bin->height - sem_fundo_bin->height * footer_cutoff;

	int pos = 0;
	for (int y = 0 ; y < height; y++){
		for (int x = 0; x < width; x++){
			if( y < y_inicial || y > y_final || x < x_inicial || x > x_final){

				pos = x + y * bytesperline;
				sem_fundo_bin->data[pos]=0;
			}
		}
	}
return 1;
}

int binarizar_1ch_8bpp(IVC *image, IVC *sem_fundo_bin, int int_fundo){

	int height = image->height;
	int bytesperline = image->bytesperline;
	int channels = image->channels;

	int pos = 0;
	int int_pixel = 0;
	int x_bin = 0;

	for (int y = 0 ; y <height; y++){
		for (int x = 0; x < bytesperline; x=x+channels){

			pos = x + y * bytesperline ;
			int_pixel = image->data[pos] + image->data[pos+1] + image->data[pos+2];

			if ( int_pixel < int_fundo*3){
				sem_fundo_bin->data[x_bin] = 255;
			}
			else{
				sem_fundo_bin->data[x_bin] = 0;
			}
			x_bin++;
		}
	}
return 1;
}

int mostrar_zona_analise(IVC *image, float lateral_cutoff, float header_cutoff, float footer_cutoff){

	int height = image->height;
	int bytesperline = image->bytesperline;
	int channels = image->channels;

	int x_inicial = image->width * channels * lateral_cutoff;
	int x_final = image->width * channels - x_inicial;
	int y_inicial = image->height * header_cutoff;
	int y_final = image->height - image->height * footer_cutoff;

	int pos = 0;
	for(int y=0 ; y< height	; y++){
		for (int x=0; x<bytesperline; x=x+channels){
			if( ((y == y_inicial || y == y_final) && x >= x_inicial && x <= x_final ) ||
				((x == x_inicial || x == x_final) && y >= y_inicial && y <= y_final ) ) {
				pos = x + bytesperline * y;
				image->data[pos] = 0;
				image->data[pos+1] = 255;
				image->data[pos+2] = 255;
			}
		}
	}
return 1;
}


// gain = 2 (prof)

// datadst = datasrc + sum/16 * gain

// max ( datadst = datasrc + sum/16 * gain ; 0) para nao ter negativos
// min ( max ( datadst = datasrc + sum/16 * gain ; 0) ; 255) para nao passar de 255

int vc_gray_highpass_filter_enhance(IVC *src, IVC *dst, int gain){


}


// usar a matriz da do 8
// no final de calcular o valor da matriz, fazer absoluto dele para nao ter negativos
// multiplicar pelo 1/9

int vc_gray_highpass_filter(IVC *src, IVC *dst){


}

int vc_gray_lowpass_gaussian_filter(IVC *src, IVC *dst){

	// gauss média (0,0) e σ=1
	int gauss[5][5] = {
				//kx
	//ky
		{1,  4,  7,  4, 1},
		{4, 16, 26, 16, 4},
		{7, 26, 41, 26, 7},
		{4, 16, 26, 16, 4},
		{1,  4,  7,  4, 1}
	};

	// atribuir valores a variáveis
	int height = src->height;
	int width = src->width;	
	int bytesperline = src->bytesperline;
	int offset = 2; // ajustar conforme necessário

	// percorrer todos os pixeis
	for (int y=0; y<height; y++){
		for (int x=0; x<width; x++){

			// para cada pixel
			int pos = x + y* bytesperline; // pixel em analise
			int somatorio = 0;			   // total para depois dividir por 273

			// eixos da matriz
			int ky =0, kx = 0;

			// para cada ponto da matriz
			for (ky - offset ; ky<=offset; ky++){		// neste caso faz 5 linhas
				for (kx - offset; kx<=offset; kx++){	// neste caso faz 5 colunas
					
					// ponto actual da matriz
					int posk = (x+kx) + (y+ky) * bytesperline;
					
					// se o ponto actual estiver dentro os limites da imagem
					if (y-ky>=0 && y+ky<height && x-kx>=0 && x+kx<=width){

						somatorio = somatorio + src->data[posk] * gauss[ky+2][kx+2];  // multiplica o valor do pixel da matriz pelo seu peso
						// OPÇAO : divisor = divisor + gauss[ky+2][kx+2]
					}
				}
			}
			
			// dividir o somatório de (valor*peso) pelo total (273)
			dst->data[pos] = somatorio / 273; // OPÇAO: somatorio / divisor
		}
	}
return 1;
}

int vc_gray_lowpass_median_filter(IVC *src, IVC *dst, int kernel){

	int width = src->width;
    int height = src->height;
    int channels = src->channels;
    int levels = src->levels;
	int bytesperline = src->bytesperline;

	int offset = (kernel-1)/2;
	int matrixSize = kernel * kernel;

	int pos = 0;
	int validos = 0;
	int mediana = 0;
	int posicao1 = 0;
	int posicao2 = 0;

	// array fixo matrixSize = 9;

	int valores[9];
	for ( int i = 0; i<matrixSize; i++){
		valores[i]=0;
	}

	for ( int y=0; y<height; y++){  	//linhas
		for (int x=0; x < width; x++){	//colunas

			pos = y * bytesperline + x * channels;  // pixel em análise

			// inicializar array
			for ( int i = 0; i<matrixSize; i++){
				valores[i]=0;
			}

			// colectar valores da máscara para um array, se estiverem dentro de limites válidos
			int kx = 0;
			int ky = 0;
			int posk = 0;
			
			int indice_array = 0;

			for (ky=-offset; ky<=offset; ky++){
				for (kx=-offset; kx<=offset; kx++){

					if((y+ky >= 0)&&(y+ky <height)&&(x+kx >= 0)&&(x+kx <width)){

						posk = (y+ky) * bytesperline + (x+kx) * channels;
						valores[indice_array] = src->data[posk];
						indice_array++ ;
					}
				}
			}

			// bubble sort
			int temp;
			for (int i=0; i < matrixSize - 1 ; i++){		// faz iterações igual ao tamanho do array -1 (chega para garantir ordenação). em cada uma fica mais um numero bubbled no final
				for (int j=0; j<matrixSize - i -1 ; j++){	// Em cada iteração percorre todas as posições que ainda não foram "bubbled" nas anteriores
					if (valores[j]<valores[j+1]){			// e escolhe a maior/menor de todas para colar na posição actual i
						temp = valores[j];
						valores[j] = valores[j+1];
						valores[j+1]=temp;
					}
				}
			}

			// encontrar mediana
			posicao1 = indice_array/2;
			posicao2 = posicao1 +1;

    		if (indice_array % 2 == 0) {			// se for par
				mediana = (valores[posicao1] + valores[posicao2])/2;
			}
			else if (indice_array % 2 != 0) {		// se for impar
				mediana = valores[posicao2];   // ex: 5 valores, 5/2 = 2,5 , posicao1=2 posicao2=3
			}

			// atribuir valor ao pixel em análise na imagem de output
			dst->data[pos]=mediana;
		}
	}
return 1;	
}

int vc_gray_lowpass_mean_filter(IVC *src, IVC *dst, int kernelsize){

	int width = src->width;
    int height = src->height;
    int channels = src->channels;
    int levels = src->levels;
	int bytesperline = src->bytesperline;

	int offset = (kernelsize-1)/2;

	int pos = 0;
	int soma = 0;
	int validos = 0;
	float media = 0;

	for ( int y=0; y<height; y++){  	//linhas
		for (int x=0; x < width; x++){	//colunas

			pos = y * bytesperline + x * channels;
			int kx = 0;
			int ky = 0;
			int posk = 0;
			media = 0; 		// reset
			soma = 0; 		// reset
			validos = 0; 	// reset

			for (ky=-offset; ky<=offset; ky++){
				for (kx=-offset; kx<=offset; kx++){

					if((y+ky >= 0)&&(y+ky <height)&&(x+kx >= 0)&&(x+kx <width)){

						posk = (y+ky) * bytesperline + (x+kx) * channels;
						soma = soma + src->data[posk];
						validos++;
					}
				}
			}
			media = (float)soma / (float)validos;
			dst->data[pos] = media;
		}
	}
return 1;	
}

int vc_grey_edge_prewitt(IVC *src, IVC *dst, float th){
	int width = src->width;
    int height = src->height;
    int channels = src->channels;
    int levels = src->levels;
	int bytesperline = src->bytesperline;
	
	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;

	int niveis_intensidade = levels+1;
	int size = width*height*channels;

	int offset=1;

	///////////////////////////////////////// DERIVADAS  /////////////////////////////////////////

	// derivada de x

	// size tem de ser valor fixo 
	float derivada_x [262144];
	int pos = 0 ;
	int matrix[3][3];

	for (int y=0; y<height; y++){ // para cada linha
		for (int x=0; x<width; x++){ // dentro de cada coluna

			pos = x + y*bytesperline;

			matrix[0][0] = pos - bytesperline - 1;
		//	matrix[0][1] = pos - bytesperline ;
			matrix[0][2] = pos - bytesperline + 1;
			matrix[1][0] = pos - 1;
			matrix[1][2] = pos + 1;
			matrix[2][0] = pos + bytesperline - 1;
		//	matrix[2][1] = pos + bytesperline;
			matrix[2][2] = pos + bytesperline + 1;

			// calcula a intensidade do pixel
			if ( (x - offset >=0) && (x + offset <= width-1) && (y - offset>=0) && (y + offset <= height-1) ){	// não é possível fazer na primeira e na última coluna porque só existem positivos ou só existem negativos
				derivada_x[pos] = 	- datasrc[matrix[0][0]] + datasrc[matrix[0][2]]
									- datasrc[matrix[1][0]] + datasrc[matrix[1][2]]
									- datasrc[matrix[2][0]] + datasrc[matrix[2][2]] ;
				derivada_x[pos] = derivada_x[pos] / 3 ;
			}
			else{ derivada_x[pos] = 0; }		// deveria ser 0 ou ficar com o valor original?
		}
	}

	// derivada de y
	float derivada_y [262144];
	pos = 0; // para reutilizar a mesma variável

	for (int x=0; x<height; x++){ // para cada coluna
		for (int y=0; y<width; y++){ // dentro de cada linha

			pos = x + y*bytesperline;

			matrix[0][0] = pos - bytesperline - 1;
			matrix[0][1] = pos - bytesperline ;
			matrix[0][2] = pos - bytesperline + 1;
		//	matrix[1][0] = pos - 1;
		//	matrix[1][2] = pos + 1;
			matrix[2][0] = pos + bytesperline - 1;
			matrix[2][1] = pos + bytesperline;
			matrix[2][2] = pos + bytesperline + 1;

			if (  (x - offset >=0) && (x + offset <= width-1) && (y - offset>=0) && (y + offset <= height-1)  ){	// não é possível fazer na primeira e na última coluna porque só existem positivos ou só existem negativos

				// calcula a intensidade do pixel
				derivada_y[pos] = 	- datasrc[matrix[0][0]] - datasrc[matrix[0][1]] - datasrc[matrix[0][2]]
									+ datasrc[matrix[2][0]] + datasrc[matrix[2][1]] + datasrc[matrix[2][2]];
								// aqui poderia verificar se cada posição da matriz indica o valor zero porque estamos a meio da linha de cima ou a meio da linha de baixo e não porque o valor é mesmo zero
								// nesses ifs individuais, teria um contador para depois saber por quantos deveriamos dividir no final ao calcular a derivada
				derivada_y[pos] = derivada_y[pos] / 3 ;
			}
			else{ derivada_y[pos] = 0; }	// deveria ser 0 ou ficar com o valor original?
		}
	}

	///////////////////////////////////////// MAGNITUDE  /////////////////////////////////////////

	// calcular magnitude do vector
	int magnitude[262144];
	for (int i=0; i<size; i++){
		magnitude[i] = round (   (1/sqrt(2))  *  sqrt(  pow(derivada_x[i],2) + pow(derivada_y[i],2)   )   );
	}

	///////////////////////////////////////// DETERMINAR T  /////////////////////////////////////////

	// // inicializar array
	int hist[256];
	for (int i = 0; i < niveis_intensidade; i++) {
        hist[i] = 0;
    }
	// contar ocorrências
	int intensidade =0;
	for (int i=0; i<size; i++){						// percorre todos os pixeis da imagem original
		intensidade = magnitude[i];					// recolhe a intensidade de cada pixel da imagem inicial (0 a 255)
		hist[intensidade] = hist[intensidade] + 1 ;	// soma mais um à contagem de ocorrências desse valor de intensidade	
	}

	// CFD      // dividir ocorrências pelo número de pixeis (poderia ser também o total de ocorrências, é igual) para obter um array de função acumulada, com valores entre 0 e 1

	// // iniciar array
	float cfd[256];
	cfd[0] = 0;				// iniciar a primeira posição em zero , as outras não é necessário

	// preencher array cfd e contar intensidade tal que ts_intensity >= ts_percent

	float ts_percent = th ;
	float last_cfd = 0;
	int ts_intensity = 0;

	for (int i=0; i<niveis_intensidade; i++){

		cfd[i] = last_cfd + (float)hist[i] / (float)size;

		if (cfd[i]>=ts_percent && last_cfd<ts_percent){       	// encontrar nível de intensidade para o qual cfd >)= ts_percent (threshold percentual de pixeis definido). Se o nível de intensidade anterior já estiver acima do ts, não executa.
			ts_intensity = i;									// porque todos os seguinte seriam verdade mas queremos apenas o primeiro nível para servir de ts_intensity
		}

		last_cfd = cfd[i];
	}

	//////////////////////////// PASSAR BORDAS PARA 0 (ZERO) (extra) ////////////////////////////

	// for(int y=0; y<height; y++){
	// 	for(int x=0; x<width; x++){
	// 		pos = x*channels + y*bytesperline ;

	// 		if( x==0 || x==width-1){
	// 			magnitude[pos]=0;
	// 		}
	// 		if( y==0 || y==height-1){
	// 			magnitude[pos]=0;
	// 		}
	// 	}
	// }

	//////////////////////////// FILTRAR MAGNITUDE POR T E CRIAR IMAGEM ////////////////////////////

	// guardar na imagem de output apenas os valores do array magnitude que são acima do ts_intensity calculado
	for (int i=0; i<size; i++){
	
		if (magnitude[i] >= ts_intensity){
			datadst[i] = 255;
		}
		else {
			datadst[i] = 0;
		}
	}

return 1;
}


// int vc_grey_edge_sobel(IVC *src, IVC *dst, float th){

// 	return 1;
// }


int vc_gray_histogram_equalization(IVC *src, IVC *dst){
	int width = src->width;
    int height = src->height;
    int channels = src->channels;
    int levels = src->levels;
	int bytesperline = src->bytesperline;
	
	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	
	int size = width*height*channels;

	// criar histograma
	int niveis_intensidade = levels +1; // por alguma razao levels tem apenas 255 níveis e não 256
	float hist[256];
	for (int i=0 ; i<niveis_intensidade ; i++){
		hist[i]=0;
	}

	// contar ocorrências para cada nível
	int intensidade = 0;
	for (int i=0; i<size; i = i + channels){
		intensidade = datasrc[i];
		hist[intensidade]+=1;
	}	

	// numero total de pixeis (para dividir o valor cumulativo por este valor. 
	// Desta forma temos um valor de 0 a 1 para cada intensidade, que é a percentagem de vezes que a intensidade é menor ou igual a isso)
	float n_pixeis = width * height;

	// criar array funçao cumulativa
	float cfd[256];
	for (int i=0; i< niveis_intensidade; i++){
		cfd[i] = 0;
	}
	
	// calcular funcao cumulativa
	float last_cfd = 0;
	for (int i=0; i< niveis_intensidade; i++){
		cfd[i] = last_cfd + ( hist[i] / n_pixeis );
		last_cfd = cfd[i];
	}

	// encontrar mínimo da cumulativa
	float cfd_min=1;							// começar no máximo. Se começar no mínimo fica mais complexo a nível de condições
	for (int i=0; i< niveis_intensidade; i++){
		if ( cfd[i] < cfd_min && cfd[i] != 0)  cfd_min = cfd[i];
	}

	// equalizar 
	for (int x=0; x< width; x++){
		for (int y=0; y< height; y++){

			int pos = x*channels + y*bytesperline;	// pixel em processamento no momento
			int original_intensity = datasrc[pos];  // é preciso ir buscar o valor de intensidade da imagem original nesse pixel, para colocar na formula, é o f(x,y)

			datadst[pos] = (int)(    ( (cfd[original_intensity]-cfd_min) / (1 - cfd_min)  ) * (niveis_intensidade -1 )   );	// formula de equalização
		}
	}

	return 1;
}

int vc_gray_histogram_show(IVC *src, IVC *dst){

	int width = dst->width;
    int height = src->height;
    int channels = src->channels;
    int levels = src->levels;

	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline = dst->bytesperline;

	int size = width*height*channels;

	int niveis_intensidade = levels + 1 ;

	// inicializar array
	int hist[256];  
	for (int i = 0; i < niveis_intensidade; i++) {
        hist[i] = 0;
    }

	// contar ocorrências
	int intensidade =0;
	for (int i=0; i<size; i++){
		intensidade = datasrc[i];
		hist[intensidade]+=1;			
	}	

	// encontrar máximo do array de ocorrencias
	int max=0;
	for (int i = 0; i < niveis_intensidade; i++) {
        if (hist[i]>max ) max = hist[i];
    }
	// normalizacao : (hist/max) * height ( int(Quant. Pixeis))
	// aqui fazemos uma regra de 3 simples para saber qual o valor no eixo dos yy (numero de pixeis contados) a mostrar para cada x (nível de intensidade)
	int hist_graf [256];
	for (int i = 0; i < niveis_intensidade; i++) {
        hist_graf[i] = 0;
    }
	for (int i=0; i<niveis_intensidade ; i++){
		hist_graf[i]=(int)(hist[i]*height/max);
	}

	// criar imagem como normalmente e preencher os pixeis com 0 e 255 para fazer as barras

	for ( int x=0; x < width; x++)   // coluna
	{
		for ( int y=0; y<height; y++ ){	// linha
			if (y >= (height-hist_graf[x]) )
			{
				datadst[x*channels+y*bytesperline] = 255;
			}
			else datadst[x*channels+y*bytesperline] = 0;
		}
	}


return 1;
}

int vc_labeled_to_grey(IVC* imageA_labelled, IVC* imageA_label_grey, int nlabels, int width, int height){

	// receber parametros das imagems
	unsigned char *datasrc = (unsigned char *)imageA_labelled->data;
	unsigned char *datadst = (unsigned char *)imageA_label_grey->data;

	// criar função da recta de intensidade consoante o número de labels
	int IntMin = 70; // valor mínimo de intensidade;
	float m = (255 - IntMin) / (nlabels -1) ;  // inclinação da recta
	int b = 70 / (m * nlabels) ; // calcular y para nlabels = 1 (interção da recta com eixo dos yy) ;

	int i=0;
	for( i=0; i<width*height; i++){
		if ( datasrc[i]!=0 ){
			datadst[i]= m * nlabels + b ;
		}
	}
return 1;
}

// Etiquetagem de blobs
// src		: Imagem binária de entrada
// dst		: Imagem grayscale (irá conter as etiquetas)
// nlabels	: Endereço de memória de uma variável, onde será armazenado o número de etiquetas encontradas.
// OVC*		: Retorna um array de estruturas de blobs (objectos), com respectivas etiquetas. É necessário libertar posteriormente esta memória.
OVC* vc_binary_blob_labelling(IVC *src, IVC *dst, int *nlabels){
	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y, a, b;
	long int i, size;
	long int posX, posA, posB, posC, posD;
	int labeltable[256] = { 0 };
	int labelarea[256] = { 0 };
	int label = 1; // Etiqueta inicial.
	int num, tmplabel;
	OVC *blobs; // Apontador para array de blobs (objectos) que será retornado desta função.

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if ((src->width != dst->width) || (src->height != dst->height) || (src->channels != dst->channels)) return NULL;
	if (channels != 1) return NULL;

	// Copia dados da imagem binária para imagem grayscale
	memcpy(datadst, datasrc, bytesperline * height);

	// Todos os pixéis de plano de fundo devem obrigatóriamente ter valor 0
	// Todos os pixéis de primeiro plano devem obrigatóriamente ter valor 255
	// Serão atribuídas etiquetas no intervalo [1,254]
	// Este algoritmo está assim limitado a 254 labels
	for (i = 0, size = bytesperline * height; i<size; i++)
	{
		if (datadst[i] != 0) datadst[i] = 255;
	}

	// Limpa os rebordos da imagem binária
	for (y = 0; y<height; y++)
	{
		datadst[y * bytesperline + 0 * channels] = 0;
		datadst[y * bytesperline + (width - 1) * channels] = 0;
	}
	for (x = 0; x<width; x++)
	{
		datadst[0 * bytesperline + x * channels] = 0;
		datadst[(height - 1) * bytesperline + x * channels] = 0;
	}

	// Efectua a etiquetagem
	for (y = 1; y<height - 1; y++)
	{
		for (x = 1; x<width - 1; x++)
		{
			// Kernel:
			// A B C
			// D X

			posA = (y - 1) * bytesperline + (x - 1) * channels; // A
			posB = (y - 1) * bytesperline + x * channels; // B
			posC = (y - 1) * bytesperline + (x + 1) * channels; // C
			posD = y * bytesperline + (x - 1) * channels; // D
			posX = y * bytesperline + x * channels; // X

			// Se o pixel foi marcado
			if (datadst[posX] != 0)
			{
				if ((datadst[posA] == 0) && (datadst[posB] == 0) && (datadst[posC] == 0) && (datadst[posD] == 0))
				{
					datadst[posX] = label;
					labeltable[label] = label;
					label++;
				}
				else
				{
					num = 255;

					// Se A está marcado
					if (datadst[posA] != 0) num = labeltable[datadst[posA]];
					// Se B está marcado, e é menor que a etiqueta "num"
					if ((datadst[posB] != 0) && (labeltable[datadst[posB]] < num)) num = labeltable[datadst[posB]];
					// Se C está marcado, e é menor que a etiqueta "num"
					if ((datadst[posC] != 0) && (labeltable[datadst[posC]] < num)) num = labeltable[datadst[posC]];
					// Se D está marcado, e é menor que a etiqueta "num"
					if ((datadst[posD] != 0) && (labeltable[datadst[posD]] < num)) num = labeltable[datadst[posD]];

					// Atribui a etiqueta ao pixel
					datadst[posX] = num;
					labeltable[num] = num;

					// Actualiza a tabela de etiquetas
					if (datadst[posA] != 0)
					{
						if (labeltable[datadst[posA]] != num)
						{
							for (tmplabel = labeltable[datadst[posA]], a = 1; a<label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
					if (datadst[posB] != 0)
					{
						if (labeltable[datadst[posB]] != num)
						{
							for (tmplabel = labeltable[datadst[posB]], a = 1; a<label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
					if (datadst[posC] != 0)
					{
						if (labeltable[datadst[posC]] != num)
						{
							for (tmplabel = labeltable[datadst[posC]], a = 1; a<label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
					if (datadst[posD] != 0)
					{
						if (labeltable[datadst[posD]] != num)
						{
							for (tmplabel = labeltable[datadst[posD]], a = 1; a<label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
				}
			}
		}
	}

	// Volta a etiquetar a imagem
	for (y = 1; y<height - 1; y++)
	{
		for (x = 1; x<width - 1; x++)
		{
			posX = y * bytesperline + x * channels; // X

			if (datadst[posX] != 0)
			{
				datadst[posX] = labeltable[datadst[posX]];
			}
		}
	}

	//printf("\nMax Label = %d\n", label);

	// Contagem do número de blobs
	// Passo 1: Eliminar, da tabela, etiquetas repetidas
	for (a = 1; a<label - 1; a++)
	{
		for (b = a + 1; b<label; b++)
		{
			if (labeltable[a] == labeltable[b]) labeltable[b] = 0;
		}
	}
	// Passo 2: Conta etiquetas e organiza a tabela de etiquetas, para que não hajam valores vazios (zero) entre etiquetas
	*nlabels = 0;
	for (a = 1; a<label; a++)
	{
		if (labeltable[a] != 0)
		{
			labeltable[*nlabels] = labeltable[a]; // Organiza tabela de etiquetas
			(*nlabels)++; // Conta etiquetas
		}
	}

	// Se não há blobs
	if (*nlabels == 0) return NULL;

	// Cria lista de blobs (objectos) e preenche a etiqueta
	blobs = (OVC *)calloc((*nlabels), sizeof(OVC));
	if (blobs != NULL)
	{
		for (a = 0; a<(*nlabels); a++) blobs[a].label = labeltable[a];
	}
	else return NULL;

	return blobs;
}

int vc_binary_blob_info(IVC *src, OVC *blobs, int nblobs)
{
	unsigned char *data = (unsigned char *)src->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y, i;
	long int pos;
	int xmin, ymin, xmax, ymax;
	long int sumx, sumy;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if (channels != 1) return 0;

	// Conta área de cada blob
	for (i = 0; i<nblobs; i++)
	{
		xmin = width - 1;
		ymin = height - 1;
		xmax = 0;
		ymax = 0;

		sumx = 0;
		sumy = 0;

		blobs[i].area = 0;

		for (y = 1; y<height - 1; y++)
		{
			for (x = 1; x<width - 1; x++)
			{
				pos = y * bytesperline + x * channels;

				if (data[pos] == blobs[i].label)
				{
					// Área
					blobs[i].area++;

					// Centro de Gravidade
					sumx += x;
					sumy += y;

					// Bounding Box
					if (xmin > x) xmin = x;
					if (ymin > y) ymin = y;
					if (xmax < x) xmax = x;
					if (ymax < y) ymax = y;

					// Perímetro
					// Se pelo menos um dos quatro vizinhos não pertence ao mesmo label, então é um pixel de contorno
					if ((data[pos - 1] != blobs[i].label) || (data[pos + 1] != blobs[i].label) || (data[pos - bytesperline] != blobs[i].label) || (data[pos + bytesperline] != blobs[i].label))
					{
						blobs[i].perimeter++;
					}
				}
			}
		}

		// Bounding Box
		blobs[i].x = xmin;
		blobs[i].y = ymin;
		blobs[i].width = (xmax - xmin) + 1;
		blobs[i].height = (ymax - ymin) + 1;

		// Centro de Gravidade
		//blobs[i].xc = (xmax - xmin) / 2;
		//blobs[i].yc = (ymax - ymin) / 2;
		blobs[i].xc = sumx / MY_MAX(blobs[i].area, 1);
		blobs[i].yc = sumy / MY_MAX(blobs[i].area, 1);
	}

	return 1;
}

int vc_gray_open(IVC *src, IVC *dst, int kernelE, int kernelD){
    // precisamos de criar uma imagem temporária para poder passar 
    IVC *temp = vc_image_new(dst->width, dst->height, dst->channels, dst->levels);

    vc_gray_erode(src, temp, kernelE);

    vc_gray_dilate(temp, dst, kernelD);

    vc_image_free(temp);
    printf("Abertuta concluida\n");

    return 1;
}

int vc_gray_close(IVC *src, IVC *dst, int kernelE, int kernelD){

    IVC *temp = vc_image_new(dst->width, dst->height, dst->channels, dst->levels);

    vc_gray_dilate(src, temp, kernelD);

    vc_gray_erode(temp, dst, kernelE);

    vc_image_free(temp);
    printf("Fecho concluido\n");

    return 1;
}

int vc_gray_erode(IVC *src, IVC *dst, int kernel){
        unsigned char *datasrc = (unsigned char *) src->data;
        unsigned char *datadst = (unsigned char *) dst->data;
        int width = src->width;
        int height= src->height;
        int bytesperline = src ->width * src->channels;
        int channels = src->channels;
        int x, y, kx, ky, min, max;
        long int pos, posk;
        int offset = (kernel-1)/ 2; // Calculo do valor do offset 
    
        if (dst->channels != 1){
            printf("Imagen secundária tem mais do que 1 canal");
            return 0;
        }
        if (src->height <= 0|| src->width <= 0 || src->height <= 0|| src->width <= 0 || src == NULL || dst == NULL) return 0;
        if (src->channels != 1 || dst->channels != 1) return 0; // Verifica se as imagens têm os canais corretos
    
        for(y = 0; y < height; y++)
        {
            for(x = 0; x <width; x++)
            {
                pos = y * bytesperline + x * channels;
    
                max = 0;
                min = 255;
    
                for(ky = -offset; ky <= offset; ky++)
                {
                    for(kx = -offset; kx <=offset; kx++)
                    {
                        if((y+ky >= 0)&&(y+ky <height)&&(x+kx >= 0)&&(x+kx <width))
                        {
                            posk = (y+ky) * bytesperline + (x+kx) * channels; 

                            // if(datasrc[posk]>max) max=datasrc[posk];
                            if(datasrc[posk]<min) min=datasrc[posk];
                        }
                    }
                }       

                datadst[pos] = min;

            }
        }
        return 1;
}

int vc_gray_dilate(IVC *src, IVC *dst, int kernel){
        unsigned char *datasrc = (unsigned char *) src->data;
        unsigned char *datadst = (unsigned char *) dst->data;
        int width = src->width;
        int height= src->height;
        int bytesperline = src ->width * src->channels;
        int channels = src->channels;
        int x, y, kx, ky, min, max;
        long int pos, posk;
        int offset = (kernel-1)/ 2; // Calculo do valor do offset 

        if (dst->channels != 1){
            printf("Imagen secundária tem mais do que 1 canal");
            return 0;
        }
        if (src->height <= 0|| src->width <= 0 || src->height <= 0|| src->width <= 0 || src == NULL || dst == NULL) return 0;
        if (src->channels != 1 || dst->channels != 1) return 0; // Verifica se as imagens têm os canais corretos
    
        for(y = 0; y < height; y++)
        {
            for(x = 0; x <width; x++)
            {
                pos = y * bytesperline + x * channels;
    
                int flag = 0;
                max = 0;
                min = 255;
    
                for(ky = -offset; ky <= offset; ky++)
                {
                    for(kx = -offset; kx <=offset; kx++)
                    {
                        if((y+ky >= 0)&&(y+ky <height)&&(x+kx >= 0)&&(x+kx <width))
                        {
                            posk = (y+ky) * bytesperline + (x+kx) * channels; 

                            if(datasrc[posk]>max) max=datasrc[posk];
                        }
                    }
                }       

   
                    datadst[pos] = max;


            }
        }
        return 1;
}

int vc_binary_close(IVC *src, IVC *dst, int kernelE, int kernelD){

    IVC *temp = vc_image_new(dst->width, dst->height, dst->channels, dst->levels);

    vc_binary_dilate(src, temp, kernelD);

    vc_binary_erode(temp, dst, kernelE);

    vc_image_free(temp);
    printf("Fecho concluido\n");

    return 1;
}

int vc_binary_open(IVC *src, IVC *dst, int kernelE, int kernelD){
    // precisamos de criar uma imagem temporária para poder passar 
    IVC *temp = vc_image_new(dst->width, dst->height, dst->channels, dst->levels);

    vc_binary_erode(src, temp, kernelE);

    vc_binary_dilate(temp, dst, kernelD);

    vc_image_free(temp);
    printf("Abertura concluida\n");

    return 1;
}

int vc_binary_erode(IVC* src, IVC* dst, int kernel){

	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y;
	int xx , yy;
	int pos_actual, pos_vizinho, valor_pixel_central;
	int offset = (kernel - 1) / 2; // para sabermos o número de pixeis na vertical (e horizontal) a avaliar, kernel -1 / 2

	for (x = 0; x<width; x++){

		for (y = 0; y<height; y++){

			pos_actual = y * bytesperline + x * channels;
			valor_pixel_central = datasrc[pos_actual]; // esta variavel recebe o valor do pixel actual. Não podemos utilizar apenas datasrc[pos_actual] porque em baixo iriamos alterar o original

			//Vizinhos
			for (yy = -offset; yy <= offset; yy++){
				for (xx = -offset; xx <= offset; xx++){

					pos_vizinho = (y + yy) * bytesperline + (x + xx) * channels;

					// se algum vizinho for negativo, o valor do pixel central é negativo
					if (	((x + xx < 0) && (x + xx < width)) || ((y + yy < 0) && (y + yy < height))  ) {

						valor_pixel_central += datasrc [pos_vizinho];		// aqui o valor da imagem original seria alterado caso não existisse a variável intermédia valor_pixel_central
					}
				}
			}
			// escrever resultado na imagem destino
			if (valor_pixel_central != 0){
				datadst[pos_actual] = 0;
			}
			else datadst[pos_actual] = 255 ;
		}
	}
return 1;
} 

int vc_binary_dilate(IVC* src, IVC* dst, int kernel){

	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y;
	int xx = 0;
	int yy = 0;
	int pos_actual, pos_vizinho, valor_pixel_central;
	int offset = (kernel - 1) / 2; // para sabermos o número de pixeis na vertical (e horizontal) a avaliar, kernel -1 / 2

	for (x = 0; x<width; x++){

		for (y = 0; y<height; y++){

			pos_actual = y * bytesperline + x * channels; // pixel actualmente em analise
			valor_pixel_central = datasrc[pos_actual]; // esta variavel recebe o valor do pixel actual. Não podemos utilizar apenas datasrc[pos_actual] porque em baixo iriamos alterar o original

			//Vizinhos
			for (yy = -offset; yy <= offset; yy++){
				for (xx = -offset; xx <= offset; xx++){

					pos_vizinho = (y + yy) * bytesperline + (x + xx) * channels;

					// se algum vizinho for positivo, o valor do pixel central é positivo
					if (	((x + xx > 0) && (x + xx < width)) || ((y + yy > 0) && (y + yy < height))  ) {
						valor_pixel_central += datasrc [pos_vizinho];			// aqui o valor da imagem original seria alterado caso não existisse a variável intermédia valor_pixel_central
					}
				}
			}
			// escrever resultado na imagem destino
			if (valor_pixel_central > 0){
				datadst[pos_actual] = 255;
			}
			else datadst[pos_actual] = 0 ;
		}

	}
return 1;
} 

int vc_gray_to_binary_niblack(IVC* src, IVC* dst, int kernel, float k)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y;
	int max, min;
	long int pos, posk;
	int counter, xx, yy;
	float mean;
	int offset;

	for (x=0; x<width; x++){
		pos = y * bytesperline + x * channels;

		max = datasrc[pos];
		min = datasrc[pos];

		float mean = 0.0f;

		//Vizinhos
		for (counter = 0, yy-offset; yy<= offset; yy++){
			for (xx = -offset; xx <= offset; xx++){
				if ((y + yy > 0) && ( y+yy <height) && (x + xx > 0) && (x + xx<width)){

					posk = (y + yy) * bytesperline + (x + xx) * channels;

					mean += (float) datasrc[posk];

					counter++;
				}
			}
		}

	}

	mean /= counter;

return 1;
}

/* 
	Niblack do Ricardo (whatsapp)

int vc_gray_to_binary_Niblack(IVC *src, IVC *dst, int kernel, float k)
{

    unsigned char *datasrc = (unsigned char *) src->data;
    unsigned char *datadst = (unsigned char *) dst->data;
    int width = src->width;
    int height= src->height;
    int bytesperline = src ->width * src->channels;
    int channels = src->channels;
    int x, y, kx, ky, min, max, contador;
    float media = 0, soma = 0;
    float threshold = 0.0;
    long int pos, posk;
    int offset = (kernel-1)/ 2; // Calculo do valor do offset 


    if (src->height <= 0|| src->width <= 0 || src->height <= 0|| src->width <= 0 || src == NULL || dst == NULL) return 0;
    if (src->channels != 1 || dst->channels != 1) return 0; // Verifica se as imagens têm os canais corretos
    

    for(y = 0; y < height; y++)
    {
        for(x = 0; x <width; x++)
        {
            pos = y * bytesperline + x * channels;

            // Reset aos valores 
            media = 0.0;
            float desvioPadrao = 0.0;


            for(ky = -offset; ky <= offset; ky++)
            {
                for(kx = -offset; kx <=offset; kx++)
                {
                    if((y+ky >= 0)&&(y+ky <height)&&(x+kx >= 0)&&(x+kx <width))
                    {
                        posk = (y+ky) * bytesperline + (x+kx) * channels; 
                        
                  
                        // Fazer a soma para encontrar a média dos pixels
                        media += (float)datasrc[posk];
                        contador++;
                    }
                }
            }

                // Média dos pixels
                media /= contador;
    
                // Reset aos valores 
                contador = 0;

                // Calcular o desvio padrão
                for(ky = -offset; ky <= offset; ky++)
                {
                    for(kx = -offset; kx <=offset; kx++)
                    {
                        if((y+ky >= 0)&&(y+ky <height)&&(x+kx >= 0)&&(x+kx <width))
                        {
                            posk = (y+ky) * bytesperline + (x+kx) * channels; 
                            
                            // soma += (float)powf(datasrc[posk] - media, 2);
                            desvioPadrao += powf(((float)datasrc[posk] - media), 2);
                            contador++;
                        }
                    }
                } 

            // Calcular o desvio padrão
            desvioPadrao = sqrtf(desvioPadrao / contador);

            // Encontrar o threshold
            threshold =  (unsigned char) media + k * desvioPadrao;
            
            if (datasrc[pos] < (unsigned)threshold) 
                datadst[pos] = 0;
            else 
                datadst[pos] = 255;
        }
    }

    printf("Feito\n");
    return 1;
}

*/
int vc_gray_to_binary_Bernsen(IVC *src, IVC *dst, int kernel)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y, kx, ky;
	int offset = (kernel - 1) / 2; //(int) floor(((double) kernel) / 2.0);
	int max, min;
	long int pos, posk;
	unsigned char threshold;
	int L = src->levels;
	int c = c;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if ((src->width != dst->width) || (src->height != dst->height) || (src->channels != dst->channels)) return 0;
	if (channels != 1) return 0;

	for (y = 0; y<height; y++)
	{
		for (x = 0; x<width; x++)
		{
			pos = y * bytesperline + x * channels;

			max = 0;
			min = 255;

			// NxM Vizinhos
			for (ky = -offset; ky <= offset; ky++)
			{
				for (kx = -offset; kx <= offset; kx++)
				{
					if ((y + ky >= 0) && (y + ky < height) && (x + kx >= 0) && (x + kx < width))
					{
						posk = (y + ky) * bytesperline + (x + kx) * channels;

						if (datasrc[posk] > max) max = datasrc[posk];
						if (datasrc[posk] < min) min = datasrc[posk];
					}
				}
			}

			if (max-min<c) threshold = 255/2;
			else threshold = 0.5 * (max + min);

			if (datasrc[pos] > threshold) datadst[pos] = 255;
			else datadst[pos] = 0;
		}
	}

	return 1;
}

/*	BERNSEN DO RICARDO (WHATSAPP)
int vc_gray_to_binary_Bersen(IVC *src, IVC *dst, int kernel, int c)
{

    unsigned char *datasrc = (unsigned char *) src->data;
    int width = src->width;
    int height= src->height;
    int bytesperline = src ->width * src->channels;
    int channels = src->channels;
    int x, y, kx, ky, min, max;
    long int pos, posk;
    int offset = (kernel-1)/ 2; // Calculo do valor do offset 
    float threshold = 0.0;

    unsigned char *datadst = (unsigned char *) dst->data;

    if (src->height <= 0|| src->width <= 0 || src->height <= 0|| src->width <= 0 || src == NULL || dst == NULL) return 0;
    if (src->channels != 1 || dst->channels != 1) return 0; // Verifica se as imagens têm os canais corretos
    

    for(y = 0; y < height; y++)
    {
        for(x = 0; x <width; x++)
        {
            pos = y * bytesperline + x * channels;


            max = 0;
            min = 255;

            for(ky = -offset; ky <= offset; ky++)
            {
                for(kx = -offset; kx <=offset; kx++)
                {
                    if((y+ky >= 0)&&(y+ky <height)&&(x+kx >= 0)&&(x+kx <width))
                    {
                        posk = (y+ky) * bytesperline + (x+kx) * channels; 
                        
                        if(datasrc[posk]>max) max=datasrc[posk];
                        if(datasrc[posk]<min) min=datasrc[posk];
                        
                    }


                }
            }       

            if(max-min <= c) 
                threshold = (unsigned char) (float) src->levels / (float)2 ;
            else 
                threshold = (unsigned char) (float) (max + min) / (float)2 ;

            if (datasrc[pos]<=(unsigned)threshold) 
                datadst[pos] = 0;
            else 
                datadst[pos] = 255;
        }
    }
    return 1;    
}

*/

// Converter de Gray para Binário (threshold automático Midpoint)
int vc_gray_to_binary_midpoint(IVC *src, IVC *dst, int kernel)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y, kx, ky;
	int offset = (kernel - 1) / 2; //(int) floor(((double) kernel) / 2.0);
	int max, min;
	long int pos, posk;
	unsigned char threshold;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if ((src->width != dst->width) || (src->height != dst->height) || (src->channels != dst->channels)) return 0;
	if (channels != 1) return 0;

	for (y = 0; y<height; y++)
	{
		for (x = 0; x<width; x++)
		{
			pos = y * bytesperline + x * channels;

			max = 0;
			min = 255;

			// NxM Vizinhos
			for (ky = -offset; ky <= offset; ky++)
			{
				for (kx = -offset; kx <= offset; kx++)
				{
					if ((y + ky >= 0) && (y + ky < height) && (x + kx >= 0) && (x + kx < width))
					{
						posk = (y + ky) * bytesperline + (x + kx) * channels;

						if (datasrc[posk] > max) max = datasrc[posk];
						if (datasrc[posk] < min) min = datasrc[posk];
					}
				}
			}
			threshold = (unsigned char)((float)(max + min) / (float)2);
			if (datasrc[pos] > threshold) datadst[pos] = 255;
			else datadst[pos] = 0;
		}
	}

	return 1;
}

float vc_gray_to_binary_global_mean(IVC *srcdst){
	unsigned char *data = (unsigned char *) srcdst->data;
	int width = srcdst->width;
	int height = srcdst->height;
	int bytesperline = srcdst->bytesperline;
	int channels = srcdst->channels;
	int x,y, total = 0, count = 0;
	long int pos;
	float average;
	

	// verificação de erros
	if ((srcdst->width<=0) || (srcdst->height <= 0) || (srcdst->data == NULL)) return 0;
	if (channels != 1) return 0;

		srcdst->levels=1;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			pos = y * bytesperline + x * channels;

			total = total + data[pos];

		}
	}

	average = total / (width*height);

	return average;

}

int vc_gray_to_binary(IVC *srcdst, IVC *dst, int threshold)
{
    unsigned char *data = (unsigned char *)srcdst->data;
    unsigned char *data_dst = (unsigned char *)dst->data;
    int width = srcdst->width;
    int height = srcdst->height;
    int bytesperline = srcdst->width * srcdst->channels;
    int channels = srcdst->channels;
    int i, size;

    if ((srcdst->width) <= 0 || (srcdst->height <= 0) || (srcdst->data == NULL)) return 0;

    if (dst->channels != 1 || srcdst->channels != 1) return 0;

    size = width * height * channels;

    for(i = 0; i < size; i = i + channels){
        if(data[i] > threshold){
            data_dst[i] = 255;
        }else{
            data_dst[i] = 0;
        }
    }

    return 1;
}

int vc_scale_gray_to_rgb(IVC *src, IVC *dst){

	int bytesperline_dst = dst->bytesperline;
	int channels_dst = dst->channels;
	int width = src -> width;
	int height = src -> height;
	int size_src = src->width * src->height * src->channels;
	int d,i;
	long int pos_src, pos_dst;
	unsigned char red[256], green[256], blue[256];
	unsigned char brilho;

	//verificação de erros
	if ((src->width <=0) || (src->height <=0) || (src->data == NULL)) return 0;
	if ((src->width != dst->width) || (src->height != dst -> height)) return 0;
	if ((src->channels != 1) || (dst->channels != 3)) return 0;

	// (255 - 0) / (192 - 128) = 255(que é 264 na verdade)/64 = 4

	// Blue to Cyan
	for (d=0, i=0; i<64; i++, d +=4){
		red[i]=0;
		green[i]= d;
		blue[i]= 255;
	}

	// Cyan to Green
	for (d=255, i=64; i<128; i++, d -=4){
		red[i]=0;
		green[i]= d;
		blue[i]= 255;
	}

	// Green to Yellow
	for (d=0, i=128; i<192; i++, d +=4){
		red[i]=d;
		green[i]= 255;
		blue[i]= 0;
	}	

	// Yellow to Red
	for (d=255, i=192; i<256; i++, d -=4){
		red[i]=255;
		green[i]= d;
		blue[i]= 0;
	}

	// percorrer os arrays red, green e blue e atribuir o valor consoante a intensidade do gray

	d = 0;
	for (int i = 0; i < size_src; d += 3, i++) {  // d é a posicao na imagem de destino
		dst->data[d]   = red[src->data[i]];
		dst->data[d+1] = green[src->data[i]];
		dst->data[d+2] = blue[src->data[i]];
	}
}

int countWhitePixels(IVC *srcdst){
	unsigned char *data = (unsigned char *)srcdst->data;
	int width = srcdst->width;
	int height = srcdst->height;
	int bytesperline = srcdst->bytesperline;
	int i,size,pixelCount;

	size = width * height;
	for (i=0; i<size; i++){
		if(data[i]=255){
			pixelCount = pixelCount +1;
		}
	}
	return pixelCount;
}

int vc_hsv_segmentation_output(IVC *srcdst, IVC *outputdst, int hmin, int hmax, int smin, int smax, int vmin, int vmax) {

	unsigned char *data = (unsigned char *)outputdst->data;
	int width = srcdst->width;
	int height = srcdst->height;
	int bytesperline = srcdst->bytesperline;
	int channels = srcdst->channels;
	int h,s,v;
	int i,size;

	//verificação de erros
	if ((srcdst->width <=0) || (srcdst->height <=0) || (srcdst->data == NULL)) return 0;
	if (channels != 3) return 0;

	size = width * height * channels;

	for (i=0; i<size; i = i + channels){
		h = (int) (((float)data[i]) / 255.0f * 360.0f);
		s = (int) (((float)data[i+1]) / 255.0f * 100.0f);
		v = (int) (((float)data[i+2]) / 255.0f * 100.0f);

		if ((h>hmin) && (h <= hmax) && (s >= smin) && (s<=smax) && (v>= vmin) && (v<= vmax)) {
			data[i]   = 255;
			data[i+1] = 255;
			data[i+2] = 255;
		}
		else{
			data[i]   = 0;
			data[i+1] = 0;
			data[i+2] = 0;
		}
	}

	return 1;
}

int vc_hsv_segmentation2(IVC *src, IVC *dst, int hmin, int hmax, int smin, int smax , int vmin, int vmax){

	int width = src->width;
	int height = src->height;
	int channels = src->channels;
	int size = width * channels * height;
	float h, s, v = 0;
	int posdst = 0;

	for (int i=0 ; i<size; i=i+channels){

			h = (float)src->data[i] * 255 / 360 ;
			s = (float)src->data[i+1] * 255 / 360 ;
			v = (float)src->data[i+2] * 255 / 360 ;

			if( h > hmin && h < hmax     &&    s > smin && s < smax     && v < vmin && v > vmax){
				dst->data[posdst] = 255;
			}
			else{
				dst->data[posdst] = 0;
			}
			posdst++ ;
	}
	return 1;
}

int vc_hsv_segmentation(IVC *srcdst, int hmin, int hmax, int smin, int smax, int vmin, int vmax)  /*falta alguma coisa*/{

	unsigned char *data = (unsigned char *)srcdst->data;
	int width = srcdst->width;
	int height = srcdst->height;
	int bytesperline = srcdst->bytesperline;
	int channels = srcdst->channels;
	int h,s,v;
	int i,size;

	//verificação de erros
	if ((srcdst->width <=0) || (srcdst->height <=0) || (srcdst->data == NULL)) return 0;
	if (channels != 3) return 0;

	size = width * height * channels;

	for (i=0; i<size; i = i + channels){
		h = (int) (((float)data[i]) / 255.0f * 360.0f);
		s = (int) (((float)data[i+1]) / 255.0f * 100.0f);
		v = (int) (((float)data[i+2]) / 255.0f * 100.0f);

		if ((h>hmin) && (h <= hmax) && (s >= smin) && (s<=smax) && (v>= vmin) && (v<= vmax)){
			data[i]   = 255;
			data[i+1] = 255;
			data[i+2] = 255;
		}
		else{
			data[i]   = 0;
			data[i+1] = 0;
			data[i+2] = 0;
		}
	}
	return 1;
}

int vc_rgb_to_hsv2 (IVC *src, IVC *dst){

	// H-hue  V-Value S-Saturation  Value = max()  Saturation = min()

	int width = src->width;
	int height = src->height;
	int channels = src->channels;
	int bytesperline = src->bytesperline;
	int size = width * channels * height;

	float min, max, value;
	float saturation, hue;
	float red, blue, green;

	for ( int i=0 ; i<size ; i=i+channels){				  
		
/* 		red = 	src->data[i];
		blue = 	src->data[i+1];
		green = src->data[i+2]; */

		blue = 	src->data[i];
		green = 	src->data[i+1];
		red = src->data[i+2];

		max = (red > green ? ( red > blue ? red : blue ) : ( green > blue ? green : blue));
		min = (red < green ? ( red < blue ? red : blue ) : ( green < blue ? green : blue));

		value = max;
		
		if ( value == 0.0f){   // precaver contra divisão por zero
			hue = 0.0f ;
			saturation = 0.0f;
		}
		else{    // precaver contra saturaçao = 0
			saturation = ( max - min) / value * 255.0f ; // saturation é entre 0 e 1 mas nós queremos entre 0 e 255

			if(saturation == 0.0f){
				hue = 0.0f;
			}
			else if	( max == red && green >= blue ){  	// if RED>BLUE && GREEN>BLUE
				hue = 60.0f * ( green - blue )  / (max-min);		// (60 * G-B) / (max-min)	
			}
			else if ( max == red && blue > green){
				hue = 360.0f + 60.0f * ( green - blue ) / (max-min);
			}
			else if ( max == green){
				hue = 120.0f + 60.0f * ( blue - red ) / (max-min);
			}
			else hue = 240.0f + 60.0f * ( red - green) / (max-min);
			
			hue = hue / 360.0f * 255.0f ; // converter de 0 a 360 para 0 a 255
		}

		dst->data[i]= (unsigned char)hue;
		dst->data[i+1] = (unsigned char)saturation;
		dst->data[i+2] = (unsigned char)value;
	}
	return 1 ;
}

int vc_rgb_to_hsv(IVC *srcdst)
{
    unsigned char *data = (unsigned char *)srcdst->data;
    int width = srcdst->width;
    int height = srcdst->height;
    int channels = srcdst->channels;
    float r,g,b,hue,saturation,value;
    float rgb_max, rgb_min;
    int i,size;

    // verificacao de rros
    if ((width <= 0) || (height <= 0) || (data == NULL)) return 0;
    if (channels != 3) return 0;

    size = width * height * channels;

    for (int i=0; i<size ; i=i+channels)
    {
        r = (float)data[i];
        g = (float)data[i+1];
        b = (float)data[i+2];

        //calcula valores maximo e minimo dos canais de cor R , G , B
        rgb_max = (r > g ? (r > b ? r:b) : (g > b? g:b));
        rgb_min = (r < g ? (r < b ? r:b) : (g < b? g:b));

        // value toma valores entre 0 e 255

        value = rgb_max; // calculate value parameter for that pixel
        
        if(value == 0.0f){  //handle exception case
            hue = 0.0f;
            saturation =0.0f;
        }
        else{
            //saturation takes values between 0 and 255
            saturation = ((rgb_max - rgb_min)/rgb_max) * 255.0f ;

            if(saturation == 0.0f)
            {
                hue = 0.0f;
            }
            else {
                // hue takes values between 0 and 360
                if ((rgb_max == r) && (g>= b)){
                    hue = 60.0f * (g-b) / (rgb_max - rgb_min);
                }
                else if ((rgb_max == r) && (b > g)){
                    hue = 360.0f + 60.0f * (g-b) / (rgb_max-rgb_min);
                }
                else if (rgb_max == g){
                    hue = 120.0f + 60.0f * (b-r) / (rgb_max-rgb_min);
                }
                else{
                    hue = 240.0f + 60.0f * (r-g) / (rgb_max-rgb_min);
                }
            }
        }

        //assign values between 0 and 255
        data[i] = (unsigned char)(hue / 360.0f * 255.0f);
        data[i + 1] = (unsigned char)(saturation);
        data[i + 2] = (unsigned char)(value);
    
    }
	return 0;
}

int vc_rgb_to_gray(IVC *src, IVC *dst){
	unsigned char *datasrc = (unsigned char *) src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;
	unsigned char *datadst = (unsigned char *) dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;
	int width = src->width;
	int height = src->height;
	int x,y;
	long int pos_src, pos_dst;
	float rf, gf, bf;

	//verificação de erros
	if(( src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if(( src->width != dst->width) || (src->height != dst->height)) return 0;
	if(( src->channels != 3) || (dst->channels != 1)) return 0;

	for(y=0; y<height; y++){
		for (x=0; x<width; x++){
			pos_src = y * bytesperline_src + x * channels_src;
			pos_dst = y * bytesperline_dst + x * channels_dst;

			rf = (float) datasrc[pos_dst];
			gf = (float) datasrc[pos_src +1];
			bf = (float) datasrc[pos_src +2];

			datadst[pos_dst] = (unsigned char) ((rf * 0.299) + (gf * 0.587) + (bf * 0.114));
		}
	}
	return 1;
}

int vc_rgb_get_green_gray(IVC *src, IVC *dst){

	int width = src->width;
	int height = src->height;
	int channels = src->channels;
	int bytesperline = src->bytesperline;

	// Extrair a componente green
	long int pos = 0;
	for (int y=0; y<height; y++){
		for (int x=0; x<width; x++){

			pos = y * bytesperline + x * channels;	// ao multiplicar por 3 estou a saltar de 3 em 3, sempre no mesmo channel

			dst->data[pos]		= src->data[pos+1]; //Make red equal to green
			dst->data[pos+1]	= src->data[pos+1]; //Make green be equal to green
			dst->data[pos+2]	= src->data[pos+1]; //Make Blue be equal to green
		}
	}
	return 1;
}

int vc_rgb_get_red_gray(IVC *src, IVC *dst){

	int width = src->width;
	int height = src->height;
	int channels = src->channels;
	int bytesperline = src->bytesperline;

	// Extrair a componente red
	long int pos = 0;
	for (int y=0; y<height; y++){
		for (int x=0; x<width; x++){

			pos = y * bytesperline + x * channels;	// ao multiplicar por 3 estou a saltar de 3 em 3, sempre no mesmo channel

			dst->data[pos]		= src->data[pos];
			dst->data[pos+1]	= src->data[pos]; //Make green be equal to red
			dst->data[pos+2]	= src->data[pos]; //Make Blue be equal to red
		}
	}
	return 1;
}

int vc_rgb_negative(IVC *src, IVC *dst){

	int width = src->width;
	int height = src->height;
	int channels = src->channels;

	int size = width * height * channels; // vai apanhar posições suficientes para cobrir todos os channels de todos os pixeis

	for ( int i=0 ; i< size; i++){
		dst->data[i] = 255 - src->data[i];
	}
}

int vc_gray_negative(IVC *src, IVC *dst){
	
	int width =         src->width;
    int height =        src->height;
    int channels =      src->channels;

    int size = width * height * channels;

    for (int i = 0; i<size; i++){
		dst->data[i] = 255 - src->data[i];
    }

return 1;
}


#pragma region Support // ##################################################

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//            FUN��ES: ALOCAR E LIBERTAR UMA IMAGEM
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Alocar mem�ria para uma imagem
IVC *vc_image_new(int width, int height, int channels, int levels)
{
	IVC *image = (IVC *) malloc(sizeof(IVC));

	if(image == NULL) return NULL;
	if((levels <= 0) || (levels > 255)) return NULL;

	image->width = width;
	image->height = height;
	image->channels = channels;
	image->levels = levels;
	image->bytesperline = image->width * image->channels;
	image->data = (unsigned char *) malloc(image->width * image->height * image->channels * sizeof(char));

	if(image->data == NULL)
	{
		return vc_image_free(image);
	}

	return image;
}

// Libertar mem�ria de uma imagem
IVC *vc_image_free(IVC *image)
{
	if(image != NULL)
	{
		if(image->data != NULL)
		{
			free(image->data);
			image->data = NULL;
		}

		free(image);
		image = NULL;
	}

	return image;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    FUN��ES: LEITURA E ESCRITA DE IMAGENS (PBM, PGM E PPM)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

char *netpbm_get_token(FILE *file, char *tok, int len)
{
	char *t;
	int c;
	
	for(;;)
	{
		while(isspace(c = getc(file)));
		if(c != '#') break;
		do c = getc(file);
		while((c != '\n') && (c != EOF));
		if(c == EOF) break;
	}
	
	t = tok;
	
	if(c != EOF)
	{
		do
		{
			*t++ = c;
			c = getc(file);
		} while((!isspace(c)) && (c != '#') && (c != EOF) && (t - tok < len - 1));
		
		if(c == '#') ungetc(c, file);
	}
	
	*t = 0;
	
	return tok;
}

long int unsigned_char_to_bit(unsigned char *datauchar, unsigned char *databit, int width, int height)
{
	int x, y;
	int countbits;
	long int pos, counttotalbytes;
	unsigned char *p = databit;

	*p = 0;
	countbits = 1;
	counttotalbytes = 0;

	for(y=0; y<height; y++)
	{
		for(x=0; x<width; x++)
		{
			pos = width * y + x;

			if(countbits <= 8)
			{
				// Numa imagem PBM:
				// 1 = Preto
				// 0 = Branco
				//*p |= (datauchar[pos] != 0) << (8 - countbits);
				
				// Na nossa imagem:
				// 1 = Branco
				// 0 = Preto
				*p |= (datauchar[pos] == 0) << (8 - countbits);

				countbits++;
			}
			if((countbits > 8) || (x == width - 1))
			{
				p++;
				*p = 0;
				countbits = 1;
				counttotalbytes++;
			}
		}
	}

	return counttotalbytes;
}

void bit_to_unsigned_char(unsigned char *databit, unsigned char *datauchar, int width, int height)
{
	int x, y;
	int countbits;
	long int pos;
	unsigned char *p = databit;

	countbits = 1;

	for(y=0; y<height; y++)
	{
		for(x=0; x<width; x++)
		{
			pos = width * y + x;

			if(countbits <= 8)
			{
				// Numa imagem PBM:
				// 1 = Preto
				// 0 = Branco
				//datauchar[pos] = (*p & (1 << (8 - countbits))) ? 1 : 0;

				// Na nossa imagem:
				// 1 = Branco
				// 0 = Preto
				datauchar[pos] = (*p & (1 << (8 - countbits))) ? 0 : 1;
				
				countbits++;
			}
			if((countbits > 8) || (x == width - 1))
			{
				p++;
				countbits = 1;
			}
		}
	}
}

IVC *vc_read_image(char *filename)
{
	FILE *file = NULL;
	IVC *image = NULL;
	unsigned char *tmp;
	char tok[20];
	long int size, sizeofbinarydata;
	int width, height, channels;
	int levels = 255;
	int v;
	
	// Abre o ficheiro
	if((file = fopen(filename, "rb")) != NULL)
	{
		// Efectua a leitura do header
		netpbm_get_token(file, tok, sizeof(tok));

		if(strcmp(tok, "P4") == 0) { channels = 1; levels = 1; }	// Se PBM (Binary [0,1])
		else if(strcmp(tok, "P5") == 0) channels = 1;				// Se PGM (Gray [0,MAX(level,255)])
		else if(strcmp(tok, "P6") == 0) channels = 3;				// Se PPM (RGB [0,MAX(level,255)])
		else
		{
			#ifdef VC_DEBUG
			printf("ERROR -> vc_read_image():\n\tFile is not a valid PBM, PGM or PPM file.\n\tBad magic number!\n");
			#endif

			fclose(file);
			return NULL;
		}
		
		if(levels == 1) // PBM
		{
			if(sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &width) != 1 || 
			   sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &height) != 1)
			{
				#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tFile is not a valid PBM file.\n\tBad size!\n");
				#endif

				fclose(file);
				return NULL;
			}

			// Aloca mem�ria para imagem
			image = vc_image_new(width, height, channels, levels);
			if(image == NULL) return NULL;

			sizeofbinarydata = (image->width / 8 + ((image->width % 8) ? 1 : 0)) * image->height;
			tmp = (unsigned char *) malloc(sizeofbinarydata);
			if(tmp == NULL) return 0;

			#ifdef VC_DEBUG
			printf("\nchannels=%d w=%d h=%d levels=%d\n", image->channels, image->width, image->height, levels);
			#endif

			if((v = fread(tmp, sizeof(unsigned char), sizeofbinarydata, file)) != sizeofbinarydata)
			{
				#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tPremature EOF on file.\n");
				#endif

				vc_image_free(image);
				fclose(file);
				free(tmp);
				return NULL;
			}

			bit_to_unsigned_char(tmp, image->data, image->width, image->height);

			free(tmp);
		}
		else // PGM ou PPM
		{
			if(sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &width) != 1 || 
			   sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &height) != 1 || 
			   sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &levels) != 1 || levels <= 0 || levels > 255)
			{
				#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tFile is not a valid PGM or PPM file.\n\tBad size!\n");
				#endif

				fclose(file);
				return NULL;
			}

			// Aloca mem�ria para imagem
			image = vc_image_new(width, height, channels, levels);
			if(image == NULL) return NULL;

			#ifdef VC_DEBUG
			printf("\nchannels=%d w=%d h=%d levels=%d\n", image->channels, image->width, image->height, levels);
			#endif

			size = image->width * image->height * image->channels;

			if((v = fread(image->data, sizeof(unsigned char), size, file)) != size)
			{
				#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tPremature EOF on file.\n");
				#endif

				vc_image_free(image);
				fclose(file);
				return NULL;
			}
		}
		
		fclose(file);
	}
	else
	{
		#ifdef VC_DEBUG
		printf("ERROR -> vc_read_image():\n\tFile not found.\n");
		#endif
	}
	
	return image;
}

int vc_write_image(char *filename, IVC *image)
{
	FILE *file = NULL;
	unsigned char *tmp;
	long int totalbytes, sizeofbinarydata;
	
	if(image == NULL) return 0;

	if((file = fopen(filename, "wb")) != NULL)
	{
		if(image->levels == 1)
		{
			sizeofbinarydata = (image->width / 8 + ((image->width % 8) ? 1 : 0)) * image->height + 1;
			tmp = (unsigned char *) malloc(sizeofbinarydata);
			if(tmp == NULL) return 0;
			
			fprintf(file, "%s %d %d\n", "P4", image->width, image->height);
			
			totalbytes = unsigned_char_to_bit(image->data, tmp, image->width, image->height);
			printf("Total = %ld\n", totalbytes);
			if(fwrite(tmp, sizeof(unsigned char), totalbytes, file) != totalbytes)
			{
				#ifdef VC_DEBUG
				fprintf(stderr, "ERROR -> vc_read_image():\n\tError writing PBM, PGM or PPM file.\n");
				#endif

				fclose(file);
				free(tmp);
				return 0;
			}

			free(tmp);
		}
		else
		{
			fprintf(file, "%s %d %d 255\n", (image->channels == 1) ? "P5" : "P6", image->width, image->height);
		
			if(fwrite(image->data, image->bytesperline, image->height, file) != image->height)
			{
				#ifdef VC_DEBUG
				fprintf(stderr, "ERROR -> vc_read_image():\n\tError writing PBM, PGM or PPM file.\n");
				#endif

				fclose(file);
				return 0;
			}
		}
		
		fclose(file);

		return 1;
	}
	
	return 0;
}

// convert image from RGB to HSV

#pragma endregion // Support ##################################################