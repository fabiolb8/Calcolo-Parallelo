%% PRODOTTO RIGHE PER COLONNE TRA DUE MATRICI

%% Dati per l'analisi dei tempi
N=[500 1000 1500 2000 2500];
tempi_cpu=zeros(1,length(N));
tempi_gpu=zeros(1,length(N));

%% Esecuzione test su cpu
for i=1:length(N)
    
    %Allocazione matrici 
    tic;
    A=rand(N(i),N(i));
    B=rand(N(i),N(i));
    C=A*B;
    tempi_cpu(i)=toc;
    
    %Esecuzione test su gpu
    gpuDevice;
    
    tic;
    A_gpu=gpuArray(A);
    B_gpu=gpuArray(B);
    C_gpu=A_gpu*B_gpu;
    %C=gather(C_gpu);
    tempi_gpu(i)=toc;
end

%% Risultati
plot(N,tempi_gpu,'b.-',N,tempi_cpu,'r.-');
xlim([N(1) N(length(N))]);
legend('gpu','cpu');




