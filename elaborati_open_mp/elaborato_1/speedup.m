%% FASE ACQUISIZIONE DATI RELATIVI ALLE PRESTAZIONI

warning('OFF', 'MATLAB:table:ModifiedAndSavedVarnames');
p=[2,4,8];
tabella_speedup = readtable('test_valutazione_integrale.xlsx', 'Range', 'B9:G12','ReadRowName', true);
tempi_img = imread('tempi.png');
figure(1)
imshow(tempi_img)
title('Tempi')

%% ANALISI PRESTAZIONI TEMPI

c = [table2array(tabella_speedup(:,1)),table2array(tabella_speedup(:,2)),table2array(tabella_speedup(:,3)),table2array(tabella_speedup(:,4)),table2array(tabella_speedup(:,5))];
figure(2)
plot(p,c(:,1),'-*',p,c(:,2),'-o',p,c(:,3),'-+',p,c(:,4),'-gs',p,c(:,5),'-md',p,p,'--','LineWidth',2)
title('Analisi speed up')
legend('N=10^4','N=10^5','N=10^6','N=10^7','N=10^8')
xlim([p(1) p(3)])
xlabel('Numero di thread');
ylabel('Speedup S(p)');
