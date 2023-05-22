# TODO

- Fazer um board dos dispositivos

  - Fazer card clicável que leve para a tela do dispositivo
  - Fazer botão para ligar ou desligar se for IRSensor OK
  - Mostrar última leitura se for DoorSensor
    - Colocar dado real

- Fazer uma tela de um dispositivo

  - Opção de deletá-lo

- Gravar histórico dos sensores de porta

  - Criar endpoint para receber leitura do sensor

- Adicionar id dos sensores nas funções de ligar e desligar

  - Parte flask OK
  - Fazer servidor wifi no esp receber o endereço mac

- Preparar servidor para receber post do DoorSensor

- Entender porque todas as blueprints estão na main

## Flask

Use "set" in Windows

- export FLASK_APP=app
- export SECRET_KEY="your secret key"
- export DATABASE_URI="postgresql://username:password@host:port/database_name"
