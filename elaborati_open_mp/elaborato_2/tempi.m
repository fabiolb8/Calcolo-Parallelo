%% FASE ACQUISIZIONE DATI RELATIVI ALLE PRESTAZIONI

warning('OFF', 'MATLAB:table:ModifiedAndSavedVarnames');
p=[2,4,8];
tabella_tempi_rmaggc = readtable('test_valutazione_elab2_OpenMp.xlsx', 'Range', 'B40:E43','ReadRowName', true);
tabella_tempi_requc = readtable('test_valutazione_elab2_OpenMp.xlsx', 'Range', 'B45:E48','ReadRowName', true);
tabella_tempi_rminc = readtable('test_valutazione_elab2_OpenMp.xlsx', 'Range', 'B50:E53','ReadRowName', true);
tempi_img = imread('tempi.png');
figure(1)
imshow(tempi_img)
title('Tempi')

%% ANALISI PRESTAZIONI TEMPI R maggiore di C

c = [table2array(tabella_tempi_rmaggc(:,1)),table2array(tabella_tempi_rmaggc(:,2)),table2array(tabella_tempi_rmaggc(:,3))];
figure(2)
plot(p,c(:,1),'-*',p,c(:,2),'-o',p,c(:,3),'-+','LineWidth',2)
title('Analisi con N=1000 e M>N')
legend('M=5000','M=50000','M=500000')
xlim([p(1) p(3)])
xlabel('Numero di thread');
ylabel('Tempo di esecuzione (secondi)');

%% ANALISI PRESTAZIONI TEMPI R uguale a C

c = [table2array(tabella_tempi_requc(:,1)),table2array(tabella_tempi_requc(:,2)),table2array(tabella_tempi_requc(:,3))];
figure(3)
plot(p,c(:,1),'-*',p,c(:,2),'-o',p,c(:,3),'-+','LineWidth',2)
title('Analisi con N=M')
legend('M=100','M=1000','M=10000')
xlim([p(1) p(3)])
xlabel('Numero di thread');
ylabel('Tempo di esecuzione (secondi)');

%% ANALISI PRESTAZIONI TEMPI R minore di C

c = [table2array(tabella_tempi_rminc(:,1)),table2array(tabella_tempi_rminc(:,2)),table2array(tabella_tempi_rminc(:,3))];
figure(4)
plot(p,c(:,1),'-*',p,c(:,2),'-o',p,c(:,3),'-+','LineWidth',2)
title('Analisi con M=1000 e M<N')
legend('N=5000','N=50000','N=500000')
xlim([p(1) p(3)])
xlabel('Numero di thread');
ylabel('Tempo di esecuzione (secondi)');