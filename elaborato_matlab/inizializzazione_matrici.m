%% Inizializzazione matrici

N = [100 500 1000 1500 2000 2500];
tempo_for_1 = zeros(1, length(N));
tempo_parfor_1 = zeros(1, length(N));

for i=1:length(N)
    
    elemento=i;
    
    tic;
    inizializzaMatriceFor(N(i),N(i),elemento);
    tempo_for_1(i) = toc;
    tic;
    inizializzaMatriceParFor(N(i),N(i),elemento);
    tempo_parfor_1(i) = toc;
end

%% Grafico

plot(N,tempo_for_1,'b.-',N,tempo_parfor_1,'r.-','LineWidth',0.8);
legend('Inizializzazione con for','Inizializzazione con parfor','Location','NorthWest');
xlim([N(1) N(length(N))]);
title('Grafico tempi');
xlabel('Dimensione matrice');
ylabel('Tempo (secondi)');