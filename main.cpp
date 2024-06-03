#include <iostream>
#include <string>
#include <chrono>
#include <opencv2\opencv.hpp>
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\videoio.hpp>

extern "C" {
#include "vc.h"
}

/* #include "vc.h" */


void vc_timer(void) {
	static bool running = false;
	static std::chrono::steady_clock::time_point previousTime = std::chrono::steady_clock::now();

	if (!running) {
		running = true;
	}
	else {
		std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
		std::chrono::steady_clock::duration elapsedTime = currentTime - previousTime;

		// Tempo em segundos.
		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(elapsedTime);
		double nseconds = time_span.count();

		std::cout << "Tempo decorrido: " << nseconds << "segundos" << std::endl;
		std::cout << "Pressione qualquer tecla para continuar...\n";
		std::cin.get();
	}
}


int main(void) {
	// V�deo
	char videofile[52] = "C:/Users/ajmr1/Documents/VCWORK/video_resistors.mp4";
	cv::VideoCapture capture;
	struct
	{
		int width, height;
		int ntotalframes;
		int fps;
		int nframe;
	} video;
	// Outros
	std::string str;
	int key = 0;

	/* Leitura de v�deo de um ficheiro */
	/* NOTA IMPORTANTE:
	O ficheiro video.avi dever� estar localizado no mesmo direct�rio que o ficheiro de c�digo fonte.
	*/
	capture.open(videofile);

	/* Em alternativa, abrir captura de v�deo pela Webcam #0 */
	//capture.open(0, cv::CAP_DSHOW); // Pode-se utilizar apenas capture.open(0);

	/* Verifica se foi poss�vel abrir o ficheiro de v�deo */
	if (!capture.isOpened())
	{
		std::cerr << "Erro ao abrir o ficheiro de v�deo!\n";
		return 1;
	}

	/* N�mero total de frames no v�deo */
	video.ntotalframes = (int)capture.get(cv::CAP_PROP_FRAME_COUNT);
	/* Frame rate do v�deo */
	video.fps = (int)capture.get(cv::CAP_PROP_FPS);
	/* Resolu��o do v�deo */
	video.width = (int)capture.get(cv::CAP_PROP_FRAME_WIDTH);
	video.height = (int)capture.get(cv::CAP_PROP_FRAME_HEIGHT);

	// inicializar variavel nframe para condição while funcionar
	video.nframe = 0;

	/* Cria uma janela para exibir o v�deo */
	cv::namedWindow("VC - VIDEO", cv::WINDOW_AUTOSIZE);

	/* Inicia o timer */
	vc_timer();

	cv::Mat frame;
	while (key != 'q' /*&& video.nframe <= 650*/) {
		/* Leitura de uma frame do v�deo */
		capture.read(frame);

		/* Verifica se conseguiu ler a frame */
		if (frame.empty()) break;

		/* N�mero da frame a processar */
		video.nframe = (int)capture.get(cv::CAP_PROP_POS_FRAMES);

		/* Exemplo de inser��o texto na frame */
		str = std::string("RESOLUCAO: ").append(std::to_string(video.width)).append("x").append(std::to_string(video.height));
		cv::putText(frame, str, cv::Point(20, 25), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 25), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		str = std::string("TOTAL DE FRAMES: ").append(std::to_string(video.ntotalframes));
		cv::putText(frame, str, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		str = std::string("FRAME RATE: ").append(std::to_string(video.fps));
		cv::putText(frame, str, cv::Point(20, 75), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 75), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		str = std::string("N. DA FRAME: ").append(std::to_string(video.nframe));
		cv::putText(frame, str, cv::Point(20, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);

 		// Cria uma nova imagem IVC do tamanho do frame do vídeo, com 3 channels e 255 níveis
		IVC *image = vc_image_new(video.width, video.height, 3, 255);
		if (image == NULL) {
			std::cerr << "Erro ao alocar memória para a imagem IVC!\n";
			break;
        }

		// Copia dados de imagem da estrutura cv::Mat para uma estrutura IVC		cv::Mat - > IVC
		memcpy(image->data, frame.data, video.width * video.height * 3);

	//#################################################################################################################
	// ZONA DE DETEÇÃO
		float lateral_cutoff = 0.26;	//      			percentagem da imagem, nas bandas laterais, sobre a qual não é importante actuar
		float header_cutoff = 0.35;  	// anterior=0.35 // percentagem da imagem, na banda superior, sobre a qual não é importante actuar
		float footer_cutoff = 0.35;  	// anterior=0.60 // percentagem da imagem, na banda inferior, sobre a qual não é importante actuar

		// Define a struct Zone de deteção com base nos cutoffs definidos
		mostrar_zona_detecao(image, lateral_cutoff, header_cutoff, footer_cutoff);

		// encontrar resistências : identificar pixeis sum(BGR)<int_fundo (fundo é branco)
		IVC *sem_fundo_bin = vc_image_new(video.width, video.height, 1, 255);
		int int_fundo = 150;  //110
		binarizar_1ch_8bpp(image, sem_fundo_bin, int_fundo);  // binariza, para imagem 1 channel 8bpp

		// aplicar preto fora da zona de deteção - optimiza o trabalho da etiquetagem
		apagar_fora_de_zona(sem_fundo_bin, lateral_cutoff, header_cutoff, footer_cutoff );

		//int n_min_pixeis_vertical = 3;
		//eliminar_cabos(sem_fundo_bin, n_min_pixeis_vertical);

		//vc_write_image("sem_fundo_bin.ppm", sem_fundo_bin);


	// LABEL
		// criar imagem para fazer os labels
		IVC *grey_labels = vc_image_new(video.width, video.height, 1, 255);

		// pesquisa a imagem original e cria os labels em outra imagem
		int nlabels = 0;	// variavel para passar por endereço para ficar com numero de blobs identificados
		OVC* array_blobs = vc_binary_blob_labelling(sem_fundo_bin, grey_labels, &nlabels);
		 vc_image_free(sem_fundo_bin);

	// FILTRAR LABELS

		int count_relevantes = 0;
		OVC *array_blobs_relevantes = 0;

	 	if (nlabels > 0) {   		// se exisitirem blobs no frame

			// analisar as características de cada blob
			vc_binary_blob_info(grey_labels, array_blobs, nlabels);

			// parametros para blob ser considerado válido
 			int area_min = 6609, area_max = 8492, altura_min = 48, altura_max = 70 , largura_min = 180 , largura_max = 345;

			array_blobs_relevantes = filter_blobs(array_blobs, nlabels, &count_relevantes,area_min, area_max, altura_min, altura_max, largura_min, largura_max);

			printf("--> Frame: %2d \t Blobs: %2d \t Potenciais resistencias: %2d \t Area: %d Altura: %d Largura: %d\n", video.nframe,nlabels, count_relevantes, array_blobs[0].area, array_blobs[0].height,  array_blobs[0].width);
		}

		if (count_relevantes>0){   // se algum blob parecer ser uma resistência relevante

			// desenhar blobs relevantes
			draw_box(array_blobs_relevantes, image, count_relevantes);

			// analisar qual a resistência de cada blob
			analisar_blobs(array_blobs_relevantes, count_relevantes,image);
		}
		
		//  Criar imagem com tamanho da zona a analisar
/* 		for ( int i = 0 ; i< nlabels; i++){

		}
		vc_image_new() */

		//  Converter para HSV e filtrar por verde (verde)
		// 	Analisar verde


		// 	Analisar Vermelho

		

			// resistencias no video
			// 1. Verde 	Azul 		Vermelho 	Dourado   	5 6 *100	 5600
			// 2. Vermelho 	Vermelho 	Castanho 	Dourado		2 2 *10		  220
			// 3. Castanho	Preto		Vermelho	Dourado		1 0 *100	 1000
			// 4. Vermelho	Vermelho	Vermelho	Dourado		2 2 *100	 2200
			// 5. Castanho	Preto		Laranja		Dourado		1 0 *1000	10000
			// 6. Castanho	Preto		Vermelho	Dourado		1 0 *100	 1000

			// Verde 	= B ?:? , G ?:? , R ?:?
			// Azul  	= B ?:? , G ?:? , R ?:?
			// Vermelho = B ?:? , G ?:? , R ?:?
			// Castanho = B ?:? , G ?:? , R ?:?
			// Preto 	= B ?:? , G ?:? , R ?:?
			// Laranja 	= B ?:? , G ?:? , R ?:?
			// Dourado  = "Os grupos podem considerar que todas as resistências possuem uma tolerância de ±5%" pelo que não é necessário avaliar


	// GERAR FRAME

		// Copia dados de imagem da estrutura IVC para uma estrutura cv::Mat		IVC -> cv::Mat
/*  		memcpy(frame.data, sem_fundo_bin->data, video.width * video.height);  // funçao para ver a imagem binaria
 		vc_image_free(sem_fundo_bin); */
 		memcpy(frame.data, image->data, video.width * video.height * 3);

		// escrever informações junto da label
		for ( int i=0 ; i < nlabels; i++){
			if ( array_blobs[i].area != 0){
				OVC current_blob = array_blobs[i] ;
				str = std::string("Area: ").append(std::to_string(current_blob.area));
				cv::putText(frame, str, cv::Point(current_blob.x, current_blob.y+125), cv::FONT_HERSHEY_SIMPLEX, 1.5, cv::Scalar(0, 0, 0), 2);
				cv::putText(frame, str, cv::Point(current_blob.x, current_blob.y+125), cv::FONT_HERSHEY_SIMPLEX, 1.5, cv::Scalar(0, 0, 255), 1);

				str = std::string("Altura: ").append(std::to_string(current_blob.height));
				cv::putText(frame, str, cv::Point(current_blob.x, current_blob.y+175), cv::FONT_HERSHEY_SIMPLEX, 1.5, cv::Scalar(0, 0, 0), 2);
				cv::putText(frame, str, cv::Point(current_blob.x, current_blob.y+175), cv::FONT_HERSHEY_SIMPLEX, 1.5, cv::Scalar(0, 0, 255), 1);

				str = std::string("Largura: ").append(std::to_string(current_blob.width));
				cv::putText(frame, str, cv::Point(current_blob.x, current_blob.y+225), cv::FONT_HERSHEY_SIMPLEX, 1.5, cv::Scalar(0, 0, 0), 2);
				cv::putText(frame, str, cv::Point(current_blob.x, current_blob.y+225), cv::FONT_HERSHEY_SIMPLEX, 1.5, cv::Scalar(0, 0, 255), 1);
			}
		}
		free(array_blobs);

		// desenhar área de deteção
		int detect_x = image->width*lateral_cutoff;
		int detect_y =  image->height*header_cutoff -25;
		str = std::string("Area de Detecao: ");
		cv::putText(frame, str, cv::Point(detect_x, detect_y), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(detect_x, detect_y), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 255), 1);

 		vc_image_free(image);

		//################################################################################################################# //

		/* Exibe a frame */
		cv::imshow("VC - VIDEO", frame);

		/* Sai da aplica��o, se o utilizador premir a tecla 'q' */
		key = cv::waitKey(1);
	}

	/* Para o timer e exibe o tempo decorrido */
	vc_timer();

	/* Fecha a janela */
	cv::destroyWindow("VC - VIDEO");

	/* Fecha o ficheiro de v�deo */
	capture.release();

	return 0;
}
