%% Inizializzazione matrici

N = [100 500 1000 1500 2000 2500];
tempo_for = zeros(1, length(N));
tempo_parfor = zeros(1, length(N));

for i=1:length(N)
    
    elemento=i;
    
    tic;
    inizializzaMatriceFor(N(i),N(i),elemento);
    tempo_for(i) = toc;
    tic;
    inizializzaMatriceParFor(N(i),N(i),elemento);
    tempo_parfor(i) = toc;
end

%% Grafico

plot(N,tempo_for,'b',N,tempo_parfor,'r','LineWidth',1.5);
legend('Tempo inizializzazione con for','Tempo inizializzazione con par for','Location','NorthWest');