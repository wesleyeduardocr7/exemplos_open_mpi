<h1 align="center">
  📦 Utilizando OpenMPI para Computação Paralela
</h1>

## 💻 Tecnologias

Esses exempplos foram desenvolvido com as seguintes tecnologias:
* [MS-MPI](https://docs.microsoft.com/en-us/message-passing-interface/microsoft-mpi)
* [Visual Studio 2019](https://visualstudio.microsoft.com/pt-br/downloads/)

## Como executar

1. Baixe e instale os aquivos do Microsoft MPI v10.1.2 (msmpisdk.msi e msmpisetup.exe) no site da Microsoft (https://www.microsoft.com/en-us/download/details.aspx?id=100593)
  1.1 Utilize as configurações padrôes de instalação
   
2. Baixe e Instale o Visual Studio 2019 versão Community
  2.1 Na instalação marque a opção Desktop development with C++
  2.2 Ao iniciar a aplicação selecione "Create a new project" e em seguida selecione "Console App"
  2.3 Informe um nome para o projeto e escolha uma pasta onde o projeto estará localizado
   
3. Configuração da Biblioteca
  3.1 Na aba Project na parte de cima selecione a última opção "Nome do Projeto" Properties...
  3.2 Selecione C/C++ -> Aditional Include Directories -> <Edit...>
  3.3 Clique em New Line e aponte para o diretório C:\Program Files (x86)\Microsoft SDKs\MPI\Include
  3.4 Depois abaixo de C/C++ selecione Linker -> Aditional Include Directories -> <Edit...> e aponte para o diretório C:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x86
  3.5 Agora selecione Linker/Input -> Aditional Dependencies -> <Edit...> e digite msmpi.lib

4. Execução de um código em OpenMPI
  4.1 Com o projeto padrão aberto (create a new project), pegue um dos códigos desse repositório e substitua no código que foi gerado por padrão
  4.2 Na parte de cima selecione a aba Build e clique em Build Solutions. Em seguida caso esteja tudo certo com o código será gerada na pasta uma pasta chamada Debug e nela o arquivo .exe do projeto
  4.3 Abra o CMD na pasta Debug e insira o comando mpiexec -n 5 "nome do arquivo do projeto".exe
  4.4 o número 5 corresponde ao número de processos a ser utilizado na execução 

<hr>

by Wesley Eduardo ✌🏽
