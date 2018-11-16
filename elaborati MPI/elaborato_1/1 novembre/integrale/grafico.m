%% Grafico della funzione integranda

F = @(x) x./(x.^2+5);
x = linspace(0, 1, 10000);
plot(x, F(x))
title('Funzione integranda (N=10000)');
xlabel('x');
ylabel('f(x)');