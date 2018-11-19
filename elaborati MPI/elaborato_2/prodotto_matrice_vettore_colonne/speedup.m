%% FASE ACQUISIZIONE DATI RELATIVI ALLE PRESTAZIONI

warning('OFF', 'MATLAB:table:ModifiedAndSavedVarnames');
p=[2,4,8,16];
tabella_speedup_rmaggc = readtable('test_valutazione_elab2_prodotto_colonne.xlsx', 'Range', 'B4:E8','ReadRowName', true);
tabella_speedup_requc = readtable('test_valutazione_elab2_prodotto_colonne.xlsx', 'Range', 'B10:E14','ReadRowName', true);
tabella_speedup_rminc = readtable('test_valutazione_elab2_prodotto_colonne.xlsx', 'Range', 'B16:E20','ReadRowName', true);
speedup_img = imread('speedup.png');
figure(1)
imshow(speedup_img)
title('Speed up')

%% ANALISI PRESTAZIONI SPEEDUP R maggiore di C

c = [table2array(tabella_speedup_rmaggc(:,1)),table2array(tabella_speedup_rmaggc(:,2)),table2array(tabella_speedup_rmaggc(:,3))];
figure(2)
plot(p,c(:,1),'-*',p,c(:,2),'-o',p,c(:,3),'-+',p,p,'--','LineWidth',2)
title('Analisi con N=1000 e M>N')
legend('M=5000','M=50000','M=500000','Speedup ideale','Location','SouthEast')
xlim([p(1) p(4)])
xlabel('Numero di processori');
ylabel('Speedup S(p)');

%% ANALISI PRESTAZIONI SPEEDUP R uguale a C

c = [table2array(tabella_speedup_requc(:,1)),table2array(tabella_speedup_requc(:,2)),table2array(tabella_speedup_requc(:,3))];
figure(3)
plot(p,c(:,1),'-*',p,c(:,2),'-o',p,c(:,3),'-+',p,p,'--','LineWidth',2)
title('Analisi con M=N')
legend('M=100','M=1000','M=10000','Speedup ideale','Location','NorthWest')
xlim([p(1) p(4)])
xlabel('Numero di processori');
ylabel('Speedup S(p)');

%% ANALISI PRESTAZIONI SPEEDUP R minore di C

c = [table2array(tabella_speedup_rminc(:,1)),table2array(tabella_speedup_rminc(:,2)),table2array(tabella_speedup_rminc(:,3))];
figure(4)
plot(p,c(:,1),'-*',p,c(:,2),'-o',p,c(:,3),'-+',p,p,'--','LineWidth',2)
title('Analisi con M=1000 e M<N')
legend('N=5000','N=50000','N=500000','Speedup ideale','Location','SouthEast')
xlim([p(1) p(4)])
xlabel('Numero di processori');
ylabel('Speedup S(p)');
