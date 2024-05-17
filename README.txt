////////////////////////////////////////////////////////////////////////
//                        VSCODE INSTRUCTIONS                         //
//                   NUNO RIBEIRO - nmribeiro@ipca.pt                 //
////////////////////////////////////////////////////////////////////////

1) Instalar/Extrair opencv para uma pasta com diretoria cujo caminho não 
possui carateres especiais.

2) Editar o "path" em variáveis de ambiente e colocar os caminhos (junta-
mente com o CMake):
          Exemplo
          - D:\opencv\build\x64\vc16\bin
          - D:\opencv\build\x64\vc16\lib
          - C:\Program Files\CMake\bin

3) Instalar o Visual Studio Community 2022 Release para obter o compilador
C++ e poder usá-lo no VSCode posteriormente.

4) Criar uma pasta de trabalho (ex: VCWORK) com os ficheiros:

          - CMakeLists.txt
          - main.cpp / codigoexemplo.cpp
          - vc.c + vc.h
          - video

5) No VSCode, fazer "Crtl+Shift+P" e escever: "CMake: Configure" e esco-
lher o kit:
          - Visual Studio Community 2022 Release - x86_amd64

6) Usar a barra inferior para executar os comandos "Build", "Debug" e "Run"

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


        