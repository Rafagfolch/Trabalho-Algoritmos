import pandas as pd # tabelas
import matplotlib.pyplot as plt # plotar gráfico


def processarDadosAuditoria(): # faz a leitura e a limpeza dos dados

    with open('dados.csv', 'r') as f: # abre o cvs e configura como read
        linhas = f.readlines()
    
    dadosTratados = []

    # limpa os ';' e  os '-' do csv
    for linha in linhas:
        if ';' in linha and 'ALGORITMO' not in linha and '---' not in linha:
            partes = linha.strip().split(';')
            if len(partes) >= 7:
                dadosTratados.append(partes[:7])
    
    # cria um data frame geral
    dfGeral = pd.DataFrame(dadosTratados, columns=['ALGORITMO', 'CENARIO', 'TAMANHO', 'TEMPO', 'COMPARACOES', 'MOVIMENTACOES', 'ESTAVEL'])
    
    # converte os textos de todas as colunas numéricas para valores numéricos reais
    dfGeral['TAMANHO'] = pd.to_numeric(dfGeral['TAMANHO'], errors='coerce')
    dfGeral['TEMPO'] = pd.to_numeric(dfGeral['TEMPO'].str.replace(',', '.'), errors='coerce')
    dfGeral['COMPARACOES'] = pd.to_numeric(dfGeral['COMPARACOES'], errors='coerce')
    dfGeral['MOVIMENTACOES'] = pd.to_numeric(dfGeral['MOVIMENTACOES'], errors='coerce')
    
    # deleta as linhas com possiveis falhas na conversão
    dfGeral = dfGeral.dropna(subset=['TAMANHO', 'ALGORITMO'])
    
    # guarda os nomes dos algoritmos
    listaAlgoritmos = dfGeral['ALGORITMO'].unique()

    # dicionário para padronizar o nome dos arquivos
    metricasMap = {
        'Tempo': 'TEMPO',
        'Comparacoes': 'COMPARACOES',
        'Movimentacoes': 'MOVIMENTACOES',
        'Estabilidade': 'ESTAVEL'
    }


    for algoritmo in listaAlgoritmos:
        # cria um df para o algoritmo atual
        dfAlgo = dfGeral[dfGeral['ALGORITMO'] == algoritmo]
        
        # define pivo, nome e gera as tabelas csv
        for nomeMetrica, coluna in metricasMap.items():
            tabelaPivot = dfAlgo.pivot_table(index='CENARIO', columns='TAMANHO', values=coluna, aggfunc='first')
            nomeArquivoCsv = f"{algoritmo}{nomeMetrica}.csv"
            tabelaPivot.to_csv(nomeArquivoCsv, sep=';')

        # configurações e geração dos gráficos 
        # permite que 3 gráficos fiquem lado a lado em uma iamgem
        fig, eixos = plt.subplots(1, 3, figsize=(20, 6))
        fig.suptitle(f'Analise de Desempenho Assintotica: {algoritmo}', fontsize=16)
        cenarios = dfAlgo['CENARIO'].unique()
        cores = {'Aleatorio': '#1f77b4', 'Crescente': '#2ca02c', 'Decrescente': '#d62728', 'Quase_Ordenado': '#ff7f0e'}

        # configura as informações dos 3 gráficos
        plotsConfig = [
            ('Comparacoes', 'COMPARACOES'),
            ('Movimentacoes', 'MOVIMENTACOES'),
            ('Tempo de Execucao (ms)', 'TEMPO')
        ]

        # preenche os três gráficos
        for idx, (titulo, colName) in enumerate(plotsConfig):
            ax = eixos[idx] # seleciona o quadro atual
            for cenario in cenarios:
                subset = dfAlgo[dfAlgo['CENARIO'] == cenario].sort_values('TAMANHO') # ordena o eixo x

                if not subset.empty: # plota a linha
                    ax.plot(subset['TAMANHO'], subset[colName], marker='o',  label=cenario, color=cores.get(cenario, 'black'), linewidth=2)
            
            # titulos e embelezamento do gráfico
            ax.set_title(titulo)
            ax.set_xlabel('Tamanho do Vetor (N)')
            ax.set_ylabel('Valor')
            ax.grid(True, linestyle='--', alpha=0.6)
            if idx == 0:
                ax.legend(title='Cenario')
        # ajusta os espaçamentos gerais
        plt.tight_layout(rect=[0, 0.03, 1, 0.95])

        # define o nome da imagem final e salva
        nomeGrafico = f"{algoritmo}Resultados.png"
        plt.savefig(nomeGrafico)
        plt.close()

# chama a função na "main"
if __name__ == '__main__':
    processarDadosAuditoria()