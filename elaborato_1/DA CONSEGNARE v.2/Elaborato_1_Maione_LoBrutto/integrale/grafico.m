%% Grafico della funzione integranda

F = @(x) 1+sin(10.*x).*(x./(x.^2+5));
x = linspace(0, 5, 1000);
plot(x, F(x))
title('Funzione integranda in [0 5]');
xlabel('x');
ylabel('f(x)');