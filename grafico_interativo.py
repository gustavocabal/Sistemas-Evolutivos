import pandas as pd
import plotly.express as px

# Lê os dados
try:
    df = pd.read_csv('dados_evolucao.csv')
except FileNotFoundError:
    print("Erro: Arquivo CSV não encontrado.")
    exit()

# Prepara os dados para o formato do Plotly (formato "longo")
# Isso junta as duas colunas de fitness em uma só, identificadas por "Tipo"
df_long = df.melt(id_vars=['Geracao'], 
                  value_vars=['MelhorFitness', 'MediaFitness'], 
                  var_name='Tipo de Sapo', value_name='Fitness')

# Cria o gráfico interativo
fig = px.line(df_long, x='Geracao', y='Fitness', color='Tipo de Sapo',
              title='Evolução Interativa dos Sapos',
              color_discrete_map={'MelhorFitness': 'green', 'MediaFitness': 'blue'})

fig.write_html("relatorio_sapos.html")

# Mostra o gráfico (abre no navegador)
fig.show()

# Se quiser salvar em arquivo HTML para mandar pro professor:
# fig.write_html("grafico_interativo.html")