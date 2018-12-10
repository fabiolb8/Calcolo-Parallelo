numCore=feature('numCores');
%%
pool=parpool(numCore);
%% %PRODOTTO MAT MAT
%% dati per tempi
N=[100 250 500 750 1000 2500 5000];
tempi_cpu=zeros(1,length(N));
tempi_gpu=zeros(1,length(N));
%% esecuzione
for i=1:length(N)
    A=rand(N(i),N(i));
    B=rand(N(i),N(i));
    
    C=zeros(N(i),N(i));
    
    %1 sequenziale
    tic;
    C=A*B;
    tempi_seq(i)=toc;
    
    gpuDev=gpuDevice;
    reset(gpuDev);
    
    %3 con gpuarray
    tic;
    A_gpu=gpuArray(A);
    B_gpu=gpuArray(B);
    C_gpu=A_gpu*B_gpu;
    tempi_gpu(i)=toc;
    
end





