numCore=feature('numCores');
%%
pool=parpool(numCore);
%% prodotto 
N=[100 500 1000 5000 10000];
tempi_seq=zeros(1,length(N));
tempi_par=zeros(1,length(N));
tempi_matlab=zeros(1,length(N));
tempi_gpu=zeros(1,length(N));
%%
for i=1:length(N)
    A=rand(N(i),N(i));
    B=rand(N(i),N(i));
    
    C=zeros(N(i),N(i));
    
    %1 sequenziale
    tic;
    C=A*B;
    tempi_seq(i)=toc;
    
    %2 com spmd
    tic;
    spmd
    C=A*B;
    end
    tempi_par(i)=toc;
end
