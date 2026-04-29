import pandas as pd
import matplotlib.pyplot as plt
import io

def processarDadosAuditoria():
    with open('dados.csv', 'r') as f:
        linhas = f.readlines()
    
    dadosTratados = []
    for linha in linhas:
        if ';' in linha and 'ALGORITMO' not in linha and '---' not in linha:
            partes = linha.strip().split(';')
            if len(partes) >= 7:
                dadosTratados.append(partes[:7])
    
    dfGeral = pd.DataFrame(dadosTratados, columns=['ALGORITMO', 'CENARIO', 'TAMANHO', 'TEMPO', 'COMPARACOES', 'MOVIMENTACOES', 'ESTAVEL'])
    
    dfGeral['TAMANHO'] = pd.to_numeric(dfGeral['TAMANHO'], errors='coerce')
    dfGeral['TEMPO'] = pd.to_numeric(dfGeral['TEMPO'].str.replace(',', '.'), errors='coerce')
    dfGeral['COMPARACOES'] = pd.to_numeric(dfGeral['COMPARACOES'], errors='coerce')
    dfGeral['MOVIMENTACOES'] = pd.to_numeric(dfGeral['MOVIMENTACOES'], errors='coerce')
    
    dfGeral = dfGeral.dropna(subset=['TAMANHO', 'ALGORITMO'])
    
    listaAlgoritmos = dfGeral['ALGORITMO'].unique()
    metricasMap = {
        'Tempo': 'TEMPO',
        'Comparacoes': 'COMPARACOES',
        'Movimentacoes': 'MOVIMENTACOES',
        'Estabilidade': 'ESTAVEL'
    }

    for algoritmo in listaAlgoritmos:
        dfAlgo = dfGeral[dfGeral['ALGORITMO'] == algoritmo]
        
        for nomeMetrica, coluna in metricasMap.items():
            tabelaPivot = dfAlgo.pivot_table(index='CENARIO', columns='TAMANHO', values=coluna, aggfunc='first')
            nomeArquivoCsv = f"{algoritmo}{nomeMetrica}.csv"
            tabelaPivot.to_csv(nomeArquivoCsv, sep=';')

        fig, eixos = plt.subplots(1, 3, figsize=(20, 6))
        fig.suptitle(f'Analise de Desempenho Assintotica: {algoritmo}', fontsize=16)
        
        cenarios = dfAlgo['CENARIO'].unique()
        cores = {'Aleatorio': '#1f77b4', 'Crescente': '#2ca02c', 'Decrescente': '#d62728', 'Quase_Ordenado': '#ff7f0e'}

        plotsConfig = [
            ('Comparacoes', 'COMPARACOES'),
            ('Movimentacoes', 'MOVIMENTACOES'),
            ('Tempo de Execucao (ms)', 'TEMPO')
        ]

        for idx, (titulo, colName) in enumerate(plotsConfig):
            ax = eixos[idx]
            for cenario in cenarios:
                subset = dfAlgo[dfAlgo['CENARIO'] == cenario].sort_values('TAMANHO')
                if not subset.empty:
                    ax.plot(subset['TAMANHO'], subset[colName], marker='o', label=cenario, color=cores.get(cenario, 'black'), linewidth=2)
            
            ax.set_title(titulo)
            ax.set_xlabel('Tamanho do Vetor (N)')
            ax.set_ylabel('Valor')
            ax.grid(True, linestyle='--', alpha=0.6)
            if idx == 0:
                ax.legend(title='Cenario')

        plt.tight_layout(rect=[0, 0.03, 1, 0.95])
        nomeGrafico = f"{algoritmo}Resultados.png"
        plt.savefig(nomeGrafico)
        plt.close()

if __name__ == '__main__':
    processarDadosAuditoria()