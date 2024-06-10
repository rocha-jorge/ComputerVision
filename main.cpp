#include <iostream>
#include <string>
#include <chrono>
#include <opencv2\opencv.hpp>
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\videoio.hpp>

#include <stdbool.h>

extern "C" {
#include "vc.h"
}

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
	// Video
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

	/* Leitura de video de um ficheiro */
	capture.open(videofile);

	/* Verifica se foi possivel abrir o ficheiro de video */
	if (!capture.isOpened())
	{
		std::cerr << "Erro ao abrir o ficheiro de video!\n";
		return 1;
	}

	/* Numero total de frames no video */
	video.ntotalframes = (int)capture.get(cv::CAP_PROP_FRAME_COUNT);
	/* Frame rate do video */
	video.fps = (int)capture.get(cv::CAP_PROP_FPS);
	/* Resolucao do video */
	video.width = (int)capture.get(cv::CAP_PROP_FRAME_WIDTH);
	video.height = (int)capture.get(cv::CAP_PROP_FRAME_HEIGHT);

	// inicializar variavel nframe para condição while funcionar
	video.nframe = 0;

	/* Cria uma janela para exibir o video */
	cv::namedWindow("VC - VIDEO", cv::WINDOW_AUTOSIZE);

	/* Inicia o timer */
	vc_timer();

	// -----------------------------------------------------------------------
	// ARRAY DE MEMORIA DE CORES
	int cores_memoria[3] = {0, 0, 0};
	int espera_apos_identificacao = 0;
	int contar_220 = 0;
	int contar_1000 = 0;
	int contar_2200 = 0;
	int contar_5600 = 0;
	int contar_10000 = 0;
	int contar_resist = 0;

	// ALOCAR ESPAÇO PARA IMAGENS (fora do while)
	IVC *image = vc_image_new(video.width, video.height, 3, 255);
    IVC *sem_fundo_bin = vc_image_new(video.width, video.height, 1, 255);
    IVC *grey_labels = vc_image_new(video.width, video.height, 1, 255);
	OVC current_blob;

	// OUTRAS VARIAVEIS
	int count_relevantes = 0;
	OVC *array_blobs_relevantes = 0;
    // -----------------------------------------------------------------------

    cv::Mat frame;

	while (key != 'q') {
		/* Leitura de uma frame do video */
		capture.read(frame);

		/* Verifica se conseguiu ler a frame */
		if (frame.empty()) break;

		/* Numero da frame a processar */
		video.nframe = (int)capture.get(cv::CAP_PROP_POS_FRAMES);

		// Exemplo de insercao texto na frame
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

		// Copia dados de imagem da estrutura cv::Mat para uma estrutura IVC		cv::Mat - > IVC
		memcpy(image->data, frame.data, video.width * video.height * 3);
 
		// ZONA DE DETEÇÃO ------------------------------------------------------------------------------------
		float lateral_cutoff = 0.26;	// percentagem da imagem, nas bandas laterais, sobre a qual não é importante actuar
		float header_cutoff = 0.35;  	// percentagem da imagem, na banda superior, sobre a qual não é importante actuar
		float footer_cutoff = 0.35;  	// percentagem da imagem, na banda inferior, sobre a qual não é importante actuar

		// Zona de detecao
		mostrar_zona_analise(image, lateral_cutoff, header_cutoff, footer_cutoff);
		
		// encontrar resistências : identificar pixeis sum(BGR)<int_fundo (fundo é branco)
		int int_fundo = 150;  //110
		binarizar_1ch_8bpp(image, sem_fundo_bin, int_fundo);  // binariza, para imagem 1 channel 8bpp

		// aplicar preto fora da zona de deteção - optimiza o trabalho da etiquetagem
		apagar_fora_de_zona(sem_fundo_bin, lateral_cutoff, header_cutoff, footer_cutoff);

		// LABEL  ------------------------------------------------------------------------------------

		// pesquisa a imagem binaria e identifica blobs
		int nlabels = 0;	// contar blobs
		OVC* array_blobs = vc_binary_blob_labelling(sem_fundo_bin, grey_labels, &nlabels);

		// FILTRAR LABELS ------------------------------------------------------------------------------------

		count_relevantes = 0;
		array_blobs_relevantes = NULL;

		if (nlabels > 0) {   		// se exisitirem blobs no frame

			// analisar as características de cada blob
			vc_binary_blob_info(grey_labels, array_blobs, nlabels);

			// parametros para blob ser considerado válido
			int area_min = 6609, area_max = 8492, altura_min = 48, altura_max = 70 , largura_min = 180 , largura_max = 345;
			
			// criar array apenas com blobs relevantes
			array_blobs_relevantes = filter_blobs(array_blobs, nlabels, &count_relevantes,area_min, area_max, altura_min, altura_max, largura_min, largura_max);
		}

		// DETERMINAR CORES  ------------------------------------------------------------------------------------
		if(espera_apos_identificacao > 0) espera_apos_identificacao-- ; // diminui um frame à espera após uma identificacao com sucesso
		
		if (count_relevantes>0){   // se algum blob parecer ser uma resistência relevante

			// ajustar blob às sombras do lado direito
			ajustar_blobs(array_blobs_relevantes, count_relevantes);

			// analisar as cores de cada blob e gravar no cores_memoria
			analisar_resistencias(array_blobs_relevantes, count_relevantes,image, cores_memoria);

			// DESENHAR  ------------------------------------------------------------------------------------
			draw_box(array_blobs_relevantes, image, count_relevantes);  // blobs relevantes

			// DETERMINAR RESISTENCIA
			if(espera_apos_identificacao == 0){  // apenas se não estiver no compasso de espera após identificação com sucesso
			
				bool resistencia = analisar_cores_memoria(cores_memoria, &contar_220, &contar_1000, &contar_2200, &contar_5600, &contar_10000);
				
				if(resistencia == true){ // se definir resist com sucesso OU se passarem x frames, reinicia a memória de cores
					contar_resist++; 									// número total de resistencias
					espera_apos_identificacao = 65;  					// apos identificar com sucesso, espera x frames para tentar novamente					for (int i = 0; i < 3; i++) cores_memoria[i] = 0;   // reset à memoria de cores
				}
			}	
		}

	// GERAR FRAME  ------------------------------------------------------------------------------------

		// Copia dados de imagem da estrutura IVC para uma estrutura cv::Mat		IVC -> cv::Mat
   		//memcpy(frame.data, sem_fundo_bin->data, video.width * video.height);  // funçao para ver a imagem binaria
 		//vc_image_free(sem_fundo_bin);

		// metodo normal
 		memcpy(frame.data, image->data, video.width * video.height * 3);
 
		// escrever informações junto da label
		for ( int i=0 ; i < count_relevantes; i++){
				current_blob = array_blobs_relevantes[i] ;
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

		// desenhar área de detecao
		int detect_x = image->width*lateral_cutoff;
		int detect_y =  image->height*header_cutoff -25;
		str = std::string("Area de Detecao: ");
		cv::putText(frame, str, cv::Point(detect_x, detect_y), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(detect_x, detect_y), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 255), 1);

		// mostrar contagem de resistencias
		str = std::string("Resistencias detectadas  : ").append(std::to_string(contar_resist));
		cv::putText(frame, str, cv::Point(20, 150), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 150), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 120, 255), 1);

		str = std::string("Resistencias    220 Ohm: ").append(std::to_string(contar_220));
		cv::putText(frame, str, cv::Point(20, 180), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 180), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 120, 255), 1);

		str = std::string("Resistencias  1 000 Ohm: ").append(std::to_string(contar_1000));
		cv::putText(frame, str, cv::Point(20, 210), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 210), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 120, 255), 1);

		str = std::string("Resistencias  2 200 Ohm: ").append(std::to_string(contar_2200));
		cv::putText(frame, str, cv::Point(20, 240), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 240), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 120, 255), 1);

		str = std::string("Resistencias  5 600 Ohm: ").append(std::to_string(contar_5600));
		cv::putText(frame, str, cv::Point(20, 270), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 270), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 120, 255), 1);
		
		str = std::string("Resistencias 10 000 Ohm: ").append(std::to_string(contar_10000));
		cv::putText(frame, str, cv::Point(20, 300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 120, 255), 1);

		str = std::string("Wait: ").append(std::to_string(espera_apos_identificacao));
		cv::putText(frame, str, cv::Point(20, 330), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 330), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 255), 1);

		/* Exibe a frame */
		cv::imshow("VC - VIDEO", frame);

		// Sai da aplicacao se utilizador premir q mas permite concluir o video em 1/3 do tempo do que o codigo key = cv::waitKey('q');
    	key = cv::waitKey(1);

		/* Sai da aplicacao, se o utilizador premir a tecla 'q' */
		//key = cv::waitKey('q');

	}

	/* Para o timer e exibe o tempo decorrido */
	vc_timer();

	/* Fecha a janela */
	cv::destroyWindow("VC - VIDEO");

	/* Fecha o ficheiro de video */
	capture.release();

	return 0;
}
