package main.ui;

import main.controle.ControladorSimulacao;

import javax.swing.JFrame;
import javax.swing.Timer;
import java.awt.BorderLayout;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

/**
 * Janela UNICA da aplicacao: controles a esquerda, visualizacao ao centro.
 *
 * HERANCA: estende {@link JFrame}. E a dona do laco de animacao - um
 * {@link Timer} Swing (~60 qps) que roda na EDT e, a cada tique:
 *   1. avanca a simulacao se o intervalo configurado passou;
 *   2. pede um novo quadro ao motor e o exibe;
 *   3. atualiza os rotulos de status/metricas.
 *
 * Como o Timer dispara na EDT e o motor foi inicializado na EDT, TODAS as
 * chamadas nativas acontecem na mesma thread - exatamente o contrato de
 * threading exigido pela biblioteca.
 */
public class JanelaPrincipal extends JFrame {

    private static final long serialVersionUID = 1L;

    private static final int INTERVALO_ANIMACAO_MS = 16; // ~60 quadros/s

    private final transient ControladorSimulacao controlador;
    private final Timer lacoAnimacao;

    public JanelaPrincipal(ControladorSimulacao controlador) {
        this.controlador = controlador;

        setTitle("AlgorithmVision");
        setLayout(new BorderLayout());

        final PainelRenderizacao painelRenderizacao = new PainelRenderizacao(controlador);
        final PainelControles painelControles = new PainelControles(controlador);

        add(painelControles, BorderLayout.WEST);
        add(painelRenderizacao, BorderLayout.CENTER);

        pack();
        setLocationRelativeTo(null);

        setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
        addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent e) {
                encerrarAplicacao();
            }
        });

        lacoAnimacao = new Timer(INTERVALO_ANIMACAO_MS, e -> {
            controlador.avancarSeNecessario();
            painelRenderizacao.atualizarQuadro();
            painelControles.atualizar();
        });
        lacoAnimacao.start();
    }

    private void encerrarAplicacao() {
        lacoAnimacao.stop();
        controlador.encerrar(); // na EDT, mesma thread do init()
        dispose();
        System.exit(0);
    }
}
