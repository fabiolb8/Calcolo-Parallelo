%% FASE ACQUISIZIONE DATI RELATIVI ALLE PRESTAZIONI

warning('OFF', 'MATLAB:table:ModifiedAndSavedVarnames');
p=[2,4,8,16];
tabella_speedup = readtable('test_valutazione_prodotto_scalare.xlsx', 'Range', 'B12:G16','ReadRowName', true);
speedup_img = imread('speedup.png');
figure(1)
image(speedup_img)
title('Speed up')

%% ANALISI PRESTAZIONI SPEEDUP

c = [table2array(tabella_speedup(:,1)),table2array(tabella_speedup(:,2)),table2array(tabella_speedup(:,3)),table2array(tabella_speedup(:,4)),table2array(tabella_speedup(:,5))];
figure(2)
plot(p,c(:,1),'-*',p,c(:,2),'-o',p,c(:,3),'-+',p,c(:,4),'-s',p,c(:,5),'-d',p,p,'--','LineWidth',2)
title('Analisi speed up')
legend('N=10^4','N=10^5','N=10^6','N=10^7','N=10^8','Speedup ideale')
xlim([p(1) p(4)])
xlabel('Numero di processori');
ylabel('Speedup S(p)');
