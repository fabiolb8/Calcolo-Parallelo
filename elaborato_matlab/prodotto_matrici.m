%% Inizializzazione matrici

N = [100 500 1000 1500 2000 2500];
tempo_cpu = zeros(1, length(N));
tempo_gpu = zeros(1, length(N));

for i=1:length(N)
    
    tic;
    A = rand(N(i),N(i));
    B = rand(N(i),N(i));
    C = A*B;
    tempo_cpu(i) = toc;
    tic;
    A_gpu = gpuArray(rand(N(i),N(i)));
    B_gpu = gpuArray(rand(N(i),N(i)));
    C_gpu = A_gpu*B_gpu;
    tempo_gpu(i) = toc;
    
end

%% Grafico

plot(N,tempo_cpu,'b',N,tempo_gpu,'r','LineWidth',1.5);
legend('CPU','GPU','Location','NorthWest');