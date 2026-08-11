# AX Control — Fase 1: Passthrough de Campo (Disbrabam)

Firmware de instalação base para o Finder Opta no painel de maturação da Disbrabam.
Opera como espelho eletromecânico puro do painel Tectermica original, sem alterar o comportamento existente.

## Mapeamento de I/O

| Entrada | Pino | Saída       | Pino | LED     |
|---------|------|-------------|------|---------|
| Refrig  | A0   | Sitrad (R1) | D0   | LED_D0  |
| Vent    | A1   | Contatora   | D1   | LED_D1  |
| Gás     | A2   | Solenoide   | D2   | LED_D2  |
| Umidif  | A3   | Solenoide   | D3   | LED_D3  |

## Comportamento

- **Passthrough puro:** cada chave física aciona diretamente o relé correspondente.
- **Debounce 50ms** em todas as entradas (proteção contra ruído/mau contato das botoeiras legadas).
- **Intertravamentos:**
  - Refrigeração força Ventilação ligada.
  - Umidificador força Ventilação ligada.
- **Gás sem timer:** solenoide segue a chave física diretamente (comportamento fiel ao painel original).
- **LEDs sincronizados com os comandos pós-intertravamento** (não com as chaves brutas).
- **Fail-safe:** tudo inicializa desligado no boot.

## O que NÃO está nesta fase

- Leitor biométrico (iDFace) — Fase 2
- Sensores analógicos (etileno, CO2, temperatura, UR) — Fase 3
- Timer de injeção de gás — previsto para Fase 3
