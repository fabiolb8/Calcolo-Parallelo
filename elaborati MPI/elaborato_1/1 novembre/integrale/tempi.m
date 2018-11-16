%% FASE ACQUISIZIONE DATI RELATIVI ALLE PRESTAZIONI

warning('OFF', 'MATLAB:table:ModifiedAndSavedVarnames');
p=[2,4,8,16];
tabella_tempi = readtable('test_valutazione_integrale.xlsx', 'Range', 'B3:G7','ReadRowName', true);
tempi_img = imread('tempi.png');
figure(1)
imshow(tempi_img)

%% ANALISI PRESTAZIONI TEMPI

c = [table2array(tabella_tempi(:,1)),table2array(tabella_tempi(:,2)),table2array(tabella_tempi(:,3)),table2array(tabella_tempi(:,4)),table2array(tabella_tempi(:,5))];
figure(2)
subplot(1,2,1),plot(p,c(:,1),'-*',p,c(:,2),'-o',p,c(:,3),'-+','LineWidth',2)
title('Analisi dettagliata da 10k a 1M')
legend('N=10^4','N=10^5','N=10^6')
xlim([p(1) p(4)])
xlabel('Numero di processori');
ylabel('Tempo di esecuzione (secondi)');
subplot(1,2,2),plot(p,c(:,4),'-gs',p,c(:,5),'-md','LineWidth',2)
title('Analisi dettagliata da 10M a 100M')
legend('N=10^7','N=10^8')
xlim([p(1) p(4)])
xlabel('Numero di processori');
ylabel('Tempo di esecuzione (secondi)');
figure(3)
plot(p,c(:,1),'-*',p,c(:,2),'-o',p,c(:,3),'-+',p,c(:,4),'-gs',p,c(:,5),'-md','LineWidth',2)
title('Analisi complessiva')
legend('N=10^4','N=10^5','N=10^6','N=10^7','N=10^8')
xlim([p(1) p(4)])
xlabel('Numero di processori');
ylabel('Tempo di esecuzione (secondi)');
