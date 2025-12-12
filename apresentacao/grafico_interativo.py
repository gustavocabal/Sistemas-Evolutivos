import pandas as pd
import plotly.express as px

# Lê os dados
try:
    df = pd.read_csv('dados_evolucao.csv') 
except FileNotFoundError:
    print("Erro: Arquivo CSV não encontrado.")
    exit()

# Pega o último dado para o título
ultimo = df.iloc[-1]
rec_atual = ultimo['RecordeHistorico']

# Transforma para formato longo (Melt)
df_long = df.melt(id_vars=['Geracao', 'GenesR', 'GenesA'], 
                  value_vars=['MelhorFitness', 'MediaFitness', 'RecordeHistorico'], 
                  var_name='Métrica', value_name='Fitness')

# Cria o gráfico
# hover_data permite mostrar as duas colunas de genes
fig = px.line(df_long, x='Geracao', y='Fitness', color='Métrica',
              title=f'Evolução (Recorde: {rec_atual:.1f})',
              hover_data=['GenesR', 'GenesA'],
              color_discrete_map={
                  'MelhorFitness': '#00CC96',   # Verde
                  'MediaFitness': '#636EFA',    # Azul
                  'RecordeHistorico': '#EF553B' # Vermelho
              })

# Ajusta o tooltip (balãozinho do mouse) para ficar legível
fig.update_traces(
    hovertemplate="<br>".join([
        "Geração: %{x}",
        "Fitness: %{y:.2f}",
        "Genes Recordista: %{customdata[0]}",
        "Genes Atual: %{customdata[1]}"
    ])
)

# Anotação estática com o melhor gene atual no topo
fig.add_annotation(
    x=0.02, y=0.98, xref="paper", yref="paper",
    text=f"<b>🧬 DNA Atual:</b> {ultimo['GenesA']}<br><b>🏆 DNA Recorde:</b> {ultimo['GenesR']}",
    showarrow=False,
    bgcolor="rgba(255, 255, 255, 0.8)", bordercolor="black"
)

fig.write_html("relatorio_completo.html")
fig.show()