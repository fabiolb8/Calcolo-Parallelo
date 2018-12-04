%% FASE ACQUISIZIONE DATI RELATIVI ALLE PRESTAZIONI

warning('OFF', 'MATLAB:table:ModifiedAndSavedVarnames');
p=[2,4,8];
tabella_efficienza = readtable('test_valutazione_integrale.xlsx', 'Range', 'B15:G18','ReadRowName', true);
efficienza_img = imread('efficienza.png');
figure(1)
imshow(efficienza_img)
title('Efficienza')

%% ANALISI PRESTAZIONI EFFICIENZA
c = [table2array(tabella_efficienza(:,1)),table2array(tabella_efficienza(:,2)),table2array(tabella_efficienza(:,3)),table2array(tabella_efficienza(:,4)),table2array(tabella_efficienza(:,5))];
figure(2)
plot(p,c(:,1),'-*',p,c(:,2),'-o',p,c(:,3),'-+',p,c(:,4),'-s',p,c(:,5),'-d',p,[1 1 1],'--','LineWidth',2)
title('Analisi efficienza')
legend('N=10^4','N=10^5','N=10^6','N=10^7','N=10^8','Efficienza ideale')
xlim([p(1) p(3)])
ylim([0 1.05])
xlabel('Numero di thread');
ylabel('Efficienza E(p)');
