%% Prodotto tra matrici

N = [100 500 1000 1500];
tempo_cpu_3 = zeros(1, length(N));
tempo_gpu_3 = zeros(1, length(N));

for i=1:length(N)
    
    %Esecuzione su cpu
    
    tic;
    A = rand(N(i),N(i));
    B = rand(N(i),N(i));
    C = A*B;
    tempo_cpu_3(i) = toc;
    
    %Esecuzione su gpu
    
    gpuDevice;
    
    tic;
    A_gpu = gpuArray(A);
    B_gpu = gpuArray(B);
    C_gpu = A_gpu*B_gpu;
    tempo_gpu_3(i) = toc;
    
    
end

%% Grafico
plot(N,tempo_cpu_3,'b.-',N,tempo_gpu_3,'r.-','LineWidth',0.6);
legend('CPU','GPU','Location','NorthWest');
xlim([N(1) N(length(N))]);
title('Grafico tempi');
xlabel('Dimensione matrice');
ylabel('Tempo (secondi)');