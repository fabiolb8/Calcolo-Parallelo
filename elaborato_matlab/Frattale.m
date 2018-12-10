%% Frattale su cpu e su gpu
N=[100 500 1000 1500 2000 2500 3000 3500 4000 4500 5000];
tempo_cpu=zeros(1,length(N));
tempo_gpu=zeros(1,length(N));
for i=1:length(N)
    tempo_cpu(i)=timeit(@()julia_cpu(1.5,-0.75,N(i),20));
    tempo_gpu(i)=timeit(@()julia_gpu(1.5,-0.75,N(i),20));
    gpuDevice;
end

%% Risultati
plot(N,tempo_gpu,'b.-',N,tempo_cpu,'r.-');
xlim([N(1) N(length(N))]);
legend('gpu','cpu');