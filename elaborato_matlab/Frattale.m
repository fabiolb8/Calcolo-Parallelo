%% Frattale su cpu e su gpu
N=[100 1000 2000 3000 4000];
tempo_cpu_2=zeros(1,length(N));
tempo_gpu_2=zeros(1,length(N));
for i=1:length(N)
    tempo_cpu_2(i)=timeit(@()julia_cpu(1.5,-0.75,N(i),20));
    tempo_gpu_2(i)=timeit(@()julia_gpu(1.5,-0.75,N(i),20));
    gpuDevice(1);
end

%% Risultati
plot(N,tempo_gpu_2,'b.-',N,tempo_cpu_2,'r.-');
xlim([N(1) N(length(N))]);
legend('GPU','CPU','Location','NorthWest');
title('Tempi frattale');
xlabel('N');
ylabel('Tempo (secondi)');