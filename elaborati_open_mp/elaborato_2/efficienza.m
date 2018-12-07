%% FASE ACQUISIZIONE DATI RELATIVI ALLE PRESTAZIONI

warning('OFF', 'MATLAB:table:ModifiedAndSavedVarnames');
p=[2,4,8];
tabella_efficienza_rmaggc = readtable('test_valutazione_elab2_OpenMp.xlsx', 'Range', 'B22:E25','ReadRowName', true);
tabella_efficienza_requc = readtable('test_valutazione_elab2_OpenMp.xlsx', 'Range', 'B27:E30','ReadRowName', true);
tabella_efficienza_rminc = readtable('test_valutazione_elab2_OpenMp.xlsx', 'Range', 'B32:E35','ReadRowName', true);
eff = imread('efficienza.png');
figure(1)
imshow(eff)
title('Efficienza')

%% ANALISI PRESTAZIONI EFFICIENZA R maggiore di C

c = [table2array(tabella_efficienza_rmaggc(:,1)),table2array(tabella_efficienza_rmaggc(:,2)),table2array(tabella_efficienza_rmaggc(:,3))];
figure(2)
plot(p,c(:,1),'-*',p,c(:,2),'-o',p,c(:,3),'-+',p,[1 1 1],'--','LineWidth',2)
title('Analisi con N=1000 e M>N')
legend('M=5000','M=50000','M=500000','Efficienza ideale','Location','SouthEast')
xlim([p(1) p(3)])
ylim([0 1.05])
xlabel('Numero di thread');
ylabel('Efficienza E(p)');

%% ANALISI PRESTAZIONI EFFICIENZA R uguale a C

c = [table2array(tabella_efficienza_requc(:,1)),table2array(tabella_efficienza_requc(:,2)),table2array(tabella_efficienza_requc(:,3))];
figure(3)
plot(p,c(:,1),'-*',p,c(:,2),'-o',p,c(:,3),'-+',p,[1 1 1],'--','LineWidth',2)
title('Analisi con M=N')
legend('M=100','M=1000','M=10000','Efficienza ideale','Location','NorthEast')
xlim([p(1) p(3)])
ylim([0 1.05])
xlabel('Numero di thread');
ylabel('Efficienza E(p)');


%% ANALISI PRESTAZIONI EFFICIENZA R minore di C

c = [table2array(tabella_efficienza_rminc(:,1)),table2array(tabella_efficienza_rminc(:,2)),table2array(tabella_efficienza_rminc(:,3))];
figure(4)
plot(p,c(:,1),'-*',p,c(:,2),'-o',p,c(:,3),'-+',p,[1 1 1],'--','LineWidth',2)
title('Analisi con M=1000 e M<N')
legend('N=5000','N=50000','N=500000','Efficienza ideale','Location','SouthWest')
xlim([p(1) p(3)])
ylim([0 1.05])
xlabel('Numero di thread');
ylabel('Efficienza E(p)');
