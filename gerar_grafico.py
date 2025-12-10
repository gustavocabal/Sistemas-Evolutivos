import pandas as pd
import matplotlib.pyplot as plt

# 1. Lê o arquivo CSV que o C++ gerou
try:
    df = pd.read_csv('dados_evolucao.csv')
except FileNotFoundError:
    print("ERRO: O arquivo 'dados_evolucao.csv' não foi encontrado na pasta!")
    exit()

# 2. Configura o tamanho da imagem
plt.figure(figsize=(10, 6))

# 3. Plota a linha do MELHOR indivíduo (Verde)
plt.plot(df['Geracao'], df['MelhorFitness'], 
         label='Melhor Sapo (Elitismo)', color='green', linewidth=2)

# 4. Plota a linha da MÉDIA da população (Azul pontilhado)
plt.plot(df['Geracao'], df['MediaFitness'], 
         label='Média da População', color='blue', linestyle='--', alpha=0.7)

# 5. Configurações visuais (Títulos e Legendas)
plt.title('Evolução do Algoritmo Genético - Sapos')
plt.xlabel('Gerações')
plt.ylabel('Fitness (Pontuação)')
plt.legend() # Mostra a legenda das linhas
plt.grid(True, linestyle=':', alpha=0.6)

# 6. Salva o gráfico como uma imagem PNG
nome_arquivo = 'resultado_evolucao.png'
plt.savefig(nome_arquivo)
print(f"Sucesso! Gráfico salvo como '{nome_arquivo}'")