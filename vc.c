//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//           INSTITUTO POLIT�CNICO DO C�VADO E DO AVE
//                          2022/2023
//             ENGENHARIA DE SISTEMAS INFORM�TICOS
//                    VIS�O POR COMPUTADOR
//
//             [  DUARTE DUQUE - dduque@ipca.pt  ]
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "vc.h"

#include <stdbool.h>

#define MY_MAX(a, b ) (a > b ? a : b)


bool analisar_cores_memoria(int *cores_memoria, int *contar_220, int *contar_1000, int *contar_2200, int *contar_5600, int *contar_10000){

	// 1- castanho 2-verde 3-preto 4-vermelho 5-laranja 6-azul
	// Dourado  = "Os grupos podem considerar que todas as resistências possuem uma tolerância de ±5%" pelo que não é necessário avaliar

	// 2. 		Vermelho 				Vermelho 				Castanho 		2 2 *10=220
	if		(cores_memoria[0]==4 && cores_memoria[1]==4 && cores_memoria[2]==1 ){	(*contar_220)++;		return true;}

	//			Vermelho				Vermelho				Vermelho		2 2 *100=2200
	else if  (cores_memoria[0]==4 && cores_memoria[1]==4 && cores_memoria[2]==4 ){	(*contar_2200)++;		return true;}

	// 			Castanho				Preto					Vermelho		1 0 *100=1000
	else if  (cores_memoria[0]==1 && cores_memoria[1]==3 && cores_memoria[2]==4 ){	(*contar_1000)++;		return true;}

	// 			Castanho				Preto					Laranja	   		1 0 *1000=10000
	else if  (cores_memoria[0]==1 && cores_memoria[1]==3 && cores_memoria[2]==5 ){	(*contar_10000)++;	return true;}

	//			Verde 					Azul 					Vermelho   		5 6 *100=5600
	else if  (cores_memoria[0]==2 && cores_memoria[1]==6 && cores_memoria[2]==4 ){	(*contar_5600)++;		return true;}

	else {	return false; }
}

int analisar_resistencias (OVC *array_blobs_relevantes, int count_relevantes, IVC *image, int *cores_memoria){

	for (int i = 0; i<count_relevantes; i++){	// para cada blob
		OVC *current_blob = &array_blobs_relevantes[i];

		// definir tamanho da sample a tirar do blob
		int sample_width = current_blob->width;
		int sample_height = 4;

		// retira a sample do blob e converte BGR->RGB
 		IVC *sample = vc_image_new(sample_width, sample_height, 3, image->levels);
		retirar_blob_RGB(image,sample,current_blob);								//vc_write_image("sampleRGB.ppm", sample);

		// converte sample para HSV
		vc_rgb_to_hsv(sample);  // hue toma o valor de 0 a 255 e não de 0 a 360		//vc_write_image("sampleHSV.ppm", sample);

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

	// cor
	int cor_1 , cor_2 , cor_3 ;
	cor_1 = cor_2 = cor_3 = 0;
 
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

	cor_1 = cor_identificar_banda1(hue, sat, val);
	if (cor_1 != 99999 ) cores_memoria[0] = cor_1; // se obtiver uma cor válida, guarda na memoria
 
	// 2a banda / seccao ---------------------------------------
   	
	hue = sat = val = hue_total = sat_total = val_total = 0;  // reiniciar contagem para reutilizar variáveis
 	contar_pixeis = 0;

	for( int y = 0 ; y < sample->height ; y++){
		for ( int x = banda2_ini*3 ; x < banda2_fin*3; x=x+3){
		
			pos = x + y * sample->bytesperline;
			hue_total += sample->data[pos] *360.0 / 255.0;
			sat_total += sample->data[pos+1];
			val_total += sample->data[pos+2];

			contar_pixeis++;
		}
	}

	hue = hue_total / contar_pixeis;
	sat = sat_total / contar_pixeis;
	val = val_total / contar_pixeis;

	cor_2 = cor_identificar_banda2(hue, sat, val);
	if (cor_2 != 99999 ) cores_memoria[1] = cor_2;
 
	// 3a banda / seccao ---------------------------------------
  	hue = sat = val = hue_total = sat_total = val_total = 0;  // reiniciar contagem para reutilizar variáveis
 	contar_pixeis = 0;
	for( int y = 0 ; y < sample->height ; y++){
		for ( int x = banda3_ini*3; x < banda3_fin*3; x=x+3){
		
			pos = x + y * sample->bytesperline;
			hue_total += sample->data[pos] *360.0 / 255.0;
			sat_total += sample->data[pos+1];
			val_total += sample->data[pos+2];

			contar_pixeis++;
		}
	}

	hue = (int)hue_total / contar_pixeis; // de 0 a 360
	sat = (int)sat_total / contar_pixeis;
	val = (int)val_total / contar_pixeis;

	cor_3 = cor_identificar_banda3(hue, sat, val);
	if (cor_3 != 99999 ) cores_memoria[2] = cor_3;
	
	return 1;
}

// nesta funçao os valores de hue já entram em 0-360
int cor_identificar_banda1(int hue, int sat, int val){

	int color = 0;

	// HUE	(0-360)				// SATURATION				// VALUE

	int black_hue_min = 0,		black_sat_min = 30 , 		black_val_min = 0;
	int black_hue_max = 45, 	black_sat_max = 133 , 		black_val_max = 255;

	int red1_hue_min =	0,		red1_sat_min = 133 ,		red1_val_min = 165 ;
	int red1_hue_max = 	45,     red1_sat_max = 167 , 		red1_val_max = 189;

	int red2_hue_min =	237,	red2_sat_min = 144,			red2_val_min = 165 ;
	int red2_hue_max = 	360, 	red2_sat_max = 167 , 		red2_val_max = 189;

	int brown_hue_min = 4, 		brown_sat_min = 79 , 		brown_val_min = 83;
	int brown_hue_max = 27, 	brown_sat_max = 138 , 		brown_val_max = 129;

	int green_hue_min = 70, 	green_sat_min = 97 , 		green_val_min = 109;
	int green_hue_max = 111, 	green_sat_max = 129 , 		green_val_max = 184;

	int blue_hue_min = 	184,	blue_sat_min = 0 , 			blue_val_min = 0;
	int blue_hue_max = 	205, 	blue_sat_max = 255 , 		blue_val_max = 255;

	int orange_hue_min = 31, 	orange_sat_min = 95 ,		orange_val_min = 0;
	int orange_hue_max = 45, 	orange_sat_max = 255 , 		orange_val_max = 255;

 	if (hue>=brown_hue_min && hue<brown_hue_max && sat >=brown_sat_min && sat<brown_sat_max && val<brown_val_max && val>=brown_val_min){
		color = 1;
	}
	else if (hue>=green_hue_min && hue<green_hue_max && sat >=green_sat_min && sat<green_sat_max){
		color = 2;
 	}
 	else if (hue>=black_hue_min && hue<black_hue_max && sat >=black_sat_min && sat<black_sat_max && val<black_val_max){
		color = 3;
	}
	else if(hue>=red1_hue_min && hue<red1_hue_max && sat >=red1_sat_min && sat<red1_sat_max && val<red1_val_max && val>=red1_val_min){
		color = 4;
	}
	else if (hue>=red2_hue_min && hue<red2_hue_max && sat >=red2_sat_min && sat<red2_sat_max && val<red2_val_max && val>=red2_val_min){
		color = 4;
	}
	else if (hue>=orange_hue_min && hue<orange_hue_max && sat >=orange_sat_min && sat<orange_sat_max){
		color = 5;
	}
    else if (hue>=blue_hue_min && hue<blue_hue_max && sat >=blue_sat_min && sat<blue_sat_max){
		color = 6;
	}
	else{
		color= 99999;
	}

	//printf("\n \t   Banda1   \t Hue(0-360): %d   \t Sat: %d   \t Value: %d   \t Color: %d", hue , sat, val, color);

return color;
}

// nesta funçao os valores de hue já entram em 0-360
int cor_identificar_banda2(int hue, int sat, int val){

	int color = 0;

	// HUE	(0-360)				// SATURATION				// VALUE

    int black_hue_min = 0,		black_sat_min = 30 , 		black_val_min = 0;
	int black_hue_max = 124, 	black_sat_max = 143 , 		black_val_max = 255;

	int red1_hue_min =	0,		red1_sat_min = 133 ,		red1_val_min = 167 ;
	int red1_hue_max = 	47, 	red1_sat_max = 236/*173*/ , red1_val_max = 257;/*189*/;

	int red2_hue_min =	237,	red2_sat_min = 144,			red2_val_min = 165 ;
	int red2_hue_max = 	360, 	red2_sat_max = 167 , 		red2_val_max = 189;

	int brown_hue_min = 4, 		brown_sat_min = 79 , 		brown_val_min = 83;
	int brown_hue_max = 27, 	brown_sat_max = 138 , 		brown_val_max = 102;

	int green_hue_min = 70, 	green_sat_min = 97 , 		green_val_min = 109;
	int green_hue_max = 111, 	green_sat_max = 129 , 		green_val_max = 184;

	int blue_hue_min = 	184,	blue_sat_min = 0 , 			blue_val_min = 0;
	int blue_hue_max = 	205, 	blue_sat_max = 255 , 		blue_val_max = 255;

	int orange_hue_min = 31, 	orange_sat_min = 95 ,		orange_val_min = 0;
	int orange_hue_max = 45, 	orange_sat_max = 255 , 		orange_val_max = 255;


 	if (hue>=brown_hue_min && hue<brown_hue_max && sat >=brown_sat_min && sat<brown_sat_max && val<brown_val_max && val>=brown_val_min){
		color = 1;
	}
	else if (hue>=green_hue_min && hue<green_hue_max && sat >=green_sat_min && sat<green_sat_max){
		color = 2;
 	}
 	else if (hue>=black_hue_min && hue<black_hue_max && sat >=black_sat_min && sat<black_sat_max && val<black_val_max){
		color = 3;
	}
	else if(hue>=red1_hue_min && hue<red1_hue_max && sat >=red1_sat_min && sat<red1_sat_max && val<red1_val_max && val>=red1_val_min){
		color = 4;
	}
	else if (hue>=red2_hue_min && hue<red2_hue_max && sat >=red2_sat_min && sat<red2_sat_max && val<red2_val_max && val>=red2_val_min){
		color = 4;
	}
	else if (hue>=orange_hue_min && hue<orange_hue_max && sat >=orange_sat_min && sat<orange_sat_max){
		color = 5;
	}
    else if (hue>=blue_hue_min && hue<blue_hue_max && sat >=blue_sat_min && sat<blue_sat_max){
		color = 6;
	}
	else{
		color= 99999;
	}

	//printf("\n \t   Banda2   \t Hue(0-360): %d   \t Sat: %d   \t Value: %d   \t Color: %d", hue , sat, val, color);

return color;
}

// nesta funçao os valores de hue já entram em 0-360
int cor_identificar_banda3(int hue, int sat, int val){

	int color = 0;

	// HUE	(0-360)				// SATURATION				// VALUE

	int black_hue_min = 0,		black_sat_min = 30 , 		black_val_min = 0;
	int black_hue_max = 124, 	black_sat_max = 53 , 		black_val_max = 174;

	int red1_hue_min =	0,		red1_sat_min = 133 ,		red1_val_min = 167 ;
	int red1_hue_max = 	33, 	red1_sat_max = 159 , 		red1_val_max = 182;

	int red2_hue_min =	237,	red2_sat_min = 144,			red2_val_min = 165 ;
	int red2_hue_max = 	360, 	red2_sat_max = 167 , 		red2_val_max = 189;

	int brown_hue_min = 4, 		brown_sat_min = 79 , 		brown_val_min = 83;
	int brown_hue_max = 27, 	brown_sat_max = 138 , 		brown_val_max = 133;

	int green_hue_min = 70, 	green_sat_min = 97 , 		green_val_min = 109;
	int green_hue_max = 111, 	green_sat_max = 129 , 		green_val_max = 184;

	int blue_hue_min = 	184,	blue_sat_min = 0 , 			blue_val_min = 0;
	int blue_hue_max = 	204, 	blue_sat_max = 255 , 		blue_val_max = 255;

	int orange_hue_min = 21, 	orange_sat_min = 142 ,		orange_val_min = 180;
	int orange_hue_max = 35, 	orange_sat_max = 168 , 		orange_val_max = 194;


 	if (hue>=brown_hue_min && hue<brown_hue_max && sat >=brown_sat_min && sat<brown_sat_max && val<brown_val_max && val>=brown_val_min){
		color = 1;
	}
	else if (hue>=green_hue_min && hue<green_hue_max && sat >=green_sat_min && sat<green_sat_max){
		color = 2;
 	}
 	else if (hue>=black_hue_min && hue<black_hue_max && sat >=black_sat_min && sat<black_sat_max && val<black_val_max){
		color = 3;
	}
	else if(hue>=red1_hue_min && hue<red1_hue_max && sat >=red1_sat_min && sat<red1_sat_max && val<red1_val_max && val>=red1_val_min){
		color = 4;
	}
	else if (hue>=red2_hue_min && hue<red2_hue_max && sat >=red2_sat_min && sat<red2_sat_max && val<red2_val_max && val>=red2_val_min){
		color = 4;
	}
	else if (hue>=orange_hue_min && hue<orange_hue_max && sat >=orange_sat_min && sat<orange_sat_max && val<orange_val_max && val>=orange_val_min){
		color = 5;
	}
    else if (hue>=blue_hue_min && hue<blue_hue_max && sat >=blue_sat_min && sat<blue_sat_max){
		color = 6;
	}
	else{
		color= 99999;
	}

	//printf("\n \t   Banda3  \t Hue(0-360): %d   \t Sat: %d   \t Value: %d   \t Color: %d", hue , sat, val, color);

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
	
	// faz com que o comprimento do blob seja igual ao dobro (*.75) do comprimento do x inicial até ao centro de massa ()
	// na prática permite diminuir o blob retirando a zona que corresponde à sombra da resistência
	for (int i = 0; i<count_relevantes; i++){
		// retirar sombras do lado direito : lado direito do centro de massa é ajustado consoante o lado esquerdo
		array_blobs_relevantes[i].width = 2* 0.75* (array_blobs_relevantes[i].xc - array_blobs_relevantes[i].x);
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


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//            FUNCOES: ALOCAR E LIBERTAR UMA IMAGEM
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Alocar memoria para uma imagem
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

// Libertar memoria de uma imagem
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
//    FUNCOES: LEITURA E ESCRITA DE IMAGENS (PBM, PGM E PPM)
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

