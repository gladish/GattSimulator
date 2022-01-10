#include "SimulatorWindow.h"

#include <QErrorMessage>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>

#include <QtBluetooth/QLowEnergyAdvertisingParameters>
#include <QtBluetooth/QLowEnergyCharacteristicData>
#include <QtBluetooth/QLowEnergyServiceData>

namespace defaults {
  QString const BeaconOnButtonText("Start Advertisement");
  QString const BeaconOffButtonText("Stop Advertisement");

  QString const SystemId("deadbeef");
  QString const ModelNumber("1");
  QString const SerialNumber("888888888888");
  QString const FirmwareRevision("1");
  QString const SoftwareRevision("1");
  QString const HardwareRevision("1");
  QString const ManufacturerName("Anil Katragadda");

  QString const QrCode("xfinity.this.that");
  QString const ProvisionStatus("0");
  QString const SimIccid("0");
  QString const ModemImei("0");
  QString const CellularSignalStrength("0");
  QString const MeshBackhaulType("0");
  QString const WiFiBackhaulStats("0");
}


namespace xle_service {
  QBluetoothUuid const UuidXleService{ QString("01b073ea-24d3-4544-b88b-9525eaa29080") };
  QBluetoothUuid const UuidQrCode{ QString("e70b8154-f71c-4dea-854b-8a88ca05ae6e") };
  QBluetoothUuid const UuidProvisionStatus{ QString("8c5310ee-73a7-4571-aee1-ca847c7e27c4") };
  QBluetoothUuid const UuidSimIccid{ QString("f4c08d33-393f-486f-9bc5-0e7cc4dd6807") };
  QBluetoothUuid const UuidModemImei{ QString("d1604db9-896e-487a-b81b-f79326c2caaf") };
  QBluetoothUuid const UuidCellularSignalStrength{ QString("28ff0021-7dca-46ca-8526-64fdaed4b935") };
  QBluetoothUuid const UuidMeshBackhaulType{ QString("83805d31-589f-4ca2-848b-219f60c15cdc") };
  QBluetoothUuid const UuidWiFiBackhaulStats{ QString("4c3629ef-2865-40be-8ef0-48d6f6cf1915") };
}

namespace device_information {
  QBluetoothUuid const UuidSystemId{ static_cast<quint16>(0x2a23) };
  QBluetoothUuid const UuidModelNumber{ static_cast<quint16>(0x2a24) };
  QBluetoothUuid const UuidSerialNumber{ static_cast<quint16>(0x2a25) };
  QBluetoothUuid const UuidFirmwareRevision{ static_cast<quint16>(0x2a26) };
  QBluetoothUuid const UuidHardwareRevision{ static_cast<quint16>(0x2a27) };
  QBluetoothUuid const UuidSoftwareRevision{ static_cast<quint16>(0x2a28) };
  QBluetoothUuid const UuidManufacturerName{ static_cast<quint16>(0x2a29) };
}

static QLowEnergyServiceData buildXleSetupService();
static QLowEnergyServiceData buildDeviceInformationService();
static char const productId[] = { 0x01, 0x03 };

static QLowEnergyCharacteristicData buildChar(
  QBluetoothUuid const & id, 
  QLowEnergyCharacteristic::PropertyTypes const & props = QLowEnergyCharacteristic::Read,
  QByteArray const & value = QByteArray());


SimulatorWindow::SimulatorWindow(QWidget * parent)
  : QMainWindow(parent)
  , m_systemId(new QLineEdit(defaults::SystemId))
  , m_modelNumber(new QLineEdit(defaults::ModelNumber))
  , m_serialNumber(new QLineEdit(defaults::SerialNumber))
  , m_firmwareRevision(new QLineEdit(defaults::FirmwareRevision))
  , m_hardwareRevision(new QLineEdit(defaults::HardwareRevision))
  , m_softwareRevision(new QLineEdit(defaults::SoftwareRevision))
  , m_manufacturerName(new QLineEdit(defaults::ManufacturerName))
  , m_qrCode(new QLineEdit(defaults::QrCode))
  , m_provisionStatus(new QLineEdit(defaults::ProvisionStatus))
  , m_simIccid(new QLineEdit(defaults::SimIccid))
  , m_modemImei(new QLineEdit(defaults::ModemImei))
  , m_cellularSignalStrength(new QLineEdit(defaults::CellularSignalStrength))
  , m_meshBackhaulType(new QLineEdit(defaults::MeshBackhaulType))
  , m_wifiBackhaulStats(new QLineEdit(defaults::WiFiBackhaulStats))
  , m_beaconRunning(false)

{
  connect(m_systemId, &QLineEdit::editingFinished, [this]() { this->updateGattSystemId(); });
  connect(m_modelNumber, &QLineEdit::editingFinished, [this]() { this->updateGattModelNumber(); });
  connect(m_serialNumber, &QLineEdit::editingFinished, [this]() { this->updateGattSerialNumber(); });
  connect(m_firmwareRevision, &QLineEdit::editingFinished, [this]() { this->updateGattFirmwareRevision(); });
  connect(m_hardwareRevision, &QLineEdit::editingFinished, [this]() { this->updateGattHardwareRevision(); });
  connect(m_softwareRevision, &QLineEdit::editingFinished, [this]() { this->updateGattSoftwareRevision(); });
  connect(m_manufacturerName, &QLineEdit::editingFinished, [this]() { this->updateGattManufacturerName(); });

  connect(m_qrCode, &QLineEdit::editingFinished, [this]() { this->updateGattQrCode(); });
  connect(m_provisionStatus, &QLineEdit::editingFinished, [this]() { this->updateGattProvisionStatus(); });
  connect(m_simIccid, &QLineEdit::editingFinished, [this]() { this->updateGattSimIccid(); });
  connect(m_modemImei, &QLineEdit::editingFinished, [this]() { this->updateGattModemImei(); });
  connect(m_cellularSignalStrength, &QLineEdit::editingFinished, [this]() { this->updateGattCellularSignalStrength(); });
  connect(m_meshBackhaulType, &QLineEdit::editingFinished, [this]() { this->updateGattMeshBackhaulType(); });
  connect(m_wifiBackhaulStats, &QLineEdit::editingFinished, [this]() { this->updateGattWiFiBackhaulStats(); }); 

  QGroupBox * deviceInfoBox = new QGroupBox("Device Information");
  QGridLayout * deviceInfoLayout = new QGridLayout();

  deviceInfoLayout->addWidget(new QLabel("SystemId"), 0, 0);
  deviceInfoLayout->addWidget(m_systemId, 0, 1);

  deviceInfoLayout->addWidget(new QLabel("ModelNumber"), 1, 0);
  deviceInfoLayout->addWidget(m_modelNumber, 1, 1);

  deviceInfoLayout->addWidget(new QLabel("SerialNumber"), 2, 0);
  deviceInfoLayout->addWidget(m_serialNumber, 2, 1);

  deviceInfoLayout->addWidget(new QLabel("FirmwareRevision"), 3, 0);
  deviceInfoLayout->addWidget(m_firmwareRevision, 3, 1);

  deviceInfoLayout->addWidget(new QLabel("HardwareRevision"), 4, 0);
  deviceInfoLayout->addWidget(m_hardwareRevision, 4, 1);

  deviceInfoLayout->addWidget(new QLabel("SoftwareRevision"), 5, 0);
  deviceInfoLayout->addWidget(m_softwareRevision, 5, 1);

  deviceInfoLayout->addWidget(new QLabel("ManufacturerName"), 6, 0);
  deviceInfoLayout->addWidget(m_manufacturerName, 6, 1);

  deviceInfoBox->setLayout(deviceInfoLayout);

  QGroupBox * deviceSpecificsBox = new QGroupBox("XLE Specifics");
  QGridLayout * deviceSpecificsLayout = new QGridLayout();

  deviceSpecificsLayout->addWidget(new QLabel("QR Code"), 0, 0);
  deviceSpecificsLayout->addWidget(m_qrCode, 0, 1);
  deviceSpecificsLayout->addWidget(new QLabel(xle_service::UuidXleService.toString()), 1, 0);

  deviceSpecificsLayout->addWidget(new QLabel("Provision Status"), 2, 0);
  deviceSpecificsLayout->addWidget(m_provisionStatus, 2, 1);
  deviceSpecificsLayout->addWidget(new QLabel(xle_service::UuidProvisionStatus.toString()), 3, 0);

  deviceSpecificsLayout->addWidget(new QLabel("SIM ICCID"), 4, 0);
  deviceSpecificsLayout->addWidget(m_simIccid, 4, 1);
  deviceSpecificsLayout->addWidget(new QLabel(xle_service::UuidSimIccid.toString()), 5, 0);

  deviceSpecificsLayout->addWidget(new QLabel("Modem IMEI"), 6, 0);
  deviceSpecificsLayout->addWidget(m_modemImei, 6, 1);
  deviceSpecificsLayout->addWidget(new QLabel(xle_service::UuidModemImei.toString()), 7, 0);

  deviceSpecificsLayout->addWidget(new QLabel("Cellular Signal Strength"), 8, 0);
  deviceSpecificsLayout->addWidget(m_cellularSignalStrength, 8, 1);
  deviceSpecificsLayout->addWidget(new QLabel(xle_service::UuidCellularSignalStrength.toString()), 9, 0);

  deviceSpecificsLayout->addWidget(new QLabel("Mesh Backhaul Type"), 10, 0);
  deviceSpecificsLayout->addWidget(m_meshBackhaulType, 10, 1);
  deviceSpecificsLayout->addWidget(new QLabel(xle_service::UuidMeshBackhaulType.toString()), 11, 0);

  deviceSpecificsLayout->addWidget(new QLabel("Wi-Fi Backhaul Stats"), 12, 0);
  deviceSpecificsLayout->addWidget(m_wifiBackhaulStats, 12, 1);
  deviceSpecificsLayout->addWidget(new QLabel(xle_service::UuidWiFiBackhaulStats.toString()), 13, 0);


  deviceSpecificsBox->setLayout(deviceSpecificsLayout);

  m_beaconOnOff = new QPushButton(defaults::BeaconOnButtonText);
  connect(m_beaconOnOff, &QPushButton::released, this, &SimulatorWindow::beaconOnOffReleased);

  QGridLayout * mainLayout = new QGridLayout();
  mainLayout->addWidget(deviceInfoBox, 0, 0);
  mainLayout->addWidget(deviceSpecificsBox, 0, 1);
  mainLayout->addWidget(m_beaconOnOff, 1, 0, 1, 2);

  QWidget * window = new QWidget();
  window->setLayout(mainLayout);

  m_statusBar = new QStatusBar();
  m_statusBar->showMessage("Disconnected");
  setStatusBar(m_statusBar);

  setCentralWidget(window);
}

void SimulatorWindow::quit()
{
  if (m_controller)
    m_controller->stopAdvertising();
  m_setupService.reset();
  m_deviceInfoService.reset();
  m_controller.reset();
}

void SimulatorWindow::beaconOnOffReleased()
{
  if (m_beaconRunning)
    stopBeacon();
  else
    startBeacon();
}

void SimulatorWindow::resetGattDB()
{
  if (!m_setupService)
    m_setupService.reset(m_controller->addService(buildXleSetupService()));

  if (!m_deviceInfoService)
    m_deviceInfoService.reset(m_controller->addService(buildDeviceInformationService()));

  updateGattSystemId();
  updateGattModelNumber();
  updateGattSerialNumber();
  updateGattFirmwareRevision();
  updateGattHardwareRevision();
  updateGattSoftwareRevision();
  updateGattManufacturerName();
  updateGattQrCode();
  updateGattProvisionStatus();
  updateGattSimIccid();
  updateGattModemImei();
  updateGattCellularSignalStrength();
  updateGattMeshBackhaulType();
  updateGattWiFiBackhaulStats();
}

void SimulatorWindow::writeDeviceInformation(QBluetoothUuid const & characteristicId, QByteArray const & value)
{
  QLowEnergyCharacteristic c = m_deviceInfoService->characteristic(characteristicId);
  m_deviceInfoService->writeCharacteristic(c, value);
}

void SimulatorWindow::writeXleInformation(QBluetoothUuid const & characteristicId, QByteArray const & value)
{
  QLowEnergyCharacteristic c = m_setupService->characteristic(characteristicId);
  m_setupService->writeCharacteristic(c, value);
}

void SimulatorWindow::updateGattQrCode()
{
  QString const s = m_qrCode->text();
  writeXleInformation(xle_service::UuidQrCode, s.toLatin1());
}

void SimulatorWindow::updateGattProvisionStatus()
{
  QString const s = m_provisionStatus->text();
  writeXleInformation(xle_service::UuidProvisionStatus, s.toLatin1());
}

void SimulatorWindow::updateGattSimIccid()
{
  QString const s = m_simIccid->text();
  writeXleInformation(xle_service::UuidSimIccid, s.toLatin1());
}
void SimulatorWindow::updateGattModemImei()
{
  QString const s = m_modemImei->text();
  writeXleInformation(xle_service::UuidModemImei, s.toLatin1());
}
void SimulatorWindow::updateGattCellularSignalStrength()
{
  QString const s = m_cellularSignalStrength->text();
  writeXleInformation(xle_service::UuidCellularSignalStrength, s.toLatin1());
}
void SimulatorWindow::updateGattMeshBackhaulType()
{
  QString const s = m_meshBackhaulType->text();
  writeXleInformation(xle_service::UuidMeshBackhaulType, s.toLatin1());
}

void SimulatorWindow::updateGattWiFiBackhaulStats()
{
  QString const s = m_wifiBackhaulStats->text();
  writeXleInformation(xle_service::UuidWiFiBackhaulStats, s.toLatin1());
}

void SimulatorWindow::updateGattSystemId()
{
  QString const s = m_systemId->text();
  writeDeviceInformation(device_information::UuidSystemId, s.toLatin1());
}

void SimulatorWindow::updateGattModelNumber()
{
  QString const s = m_modelNumber->text();
  writeDeviceInformation(device_information::UuidModelNumber, s.toLatin1());
}

void SimulatorWindow::updateGattSerialNumber()
{
  QString const s = m_serialNumber->text();
  writeDeviceInformation(device_information::UuidSerialNumber, s.toLatin1());
}

void SimulatorWindow::updateGattFirmwareRevision()
{
  QString const s = m_firmwareRevision->text();
  writeDeviceInformation(device_information::UuidFirmwareRevision, s.toLatin1());
}

void SimulatorWindow::updateGattHardwareRevision()
{
  QString const s = m_hardwareRevision->text();
  writeDeviceInformation(device_information::UuidHardwareRevision, s.toLatin1());
}

void SimulatorWindow::updateGattSoftwareRevision()
{
  QString const s = m_softwareRevision->text();
  writeDeviceInformation(device_information::UuidSoftwareRevision, s.toLatin1());
}

void SimulatorWindow::updateGattManufacturerName()
{
  QString const s = m_manufacturerName->text();
  writeDeviceInformation(device_information::UuidManufacturerName, s.toLatin1());
}

QByteArray SimulatorWindow::getDeviceSerialNumber() const
{
  QByteArray serialNumber;
  QString s = m_serialNumber->text();
  if (s.isEmpty())
    s = "123456789abc";
  if (s.size() > 12)
    throw QString("serial number needs to be 12 hexidecimal characters");

  for (QChar c : s) {
    int n = c.toLower().toLatin1() - static_cast<int>('0');
    if (n < 0)
      throw QString("'%1' is not a valid decimal digit.").arg(c);
    if (n > 9)
      throw QString("'%1' is not good. Only decimal digits allowed.").arg(c);
    serialNumber.append(n);
  }

  return serialNumber;
}

void SimulatorWindow::startBeacon()
{
  if (!m_controller) {
    m_controller.reset(QLowEnergyController::createPeripheral());
    connect(m_controller.data(), &QLowEnergyController::connected, this, &SimulatorWindow::onControllerConnect);
    connect(m_controller.data(), &QLowEnergyController::disconnected, this, &SimulatorWindow::onControllerDisconnect);
  }

  QLowEnergyAdvertisingData advertisingData;
  advertisingData.setDiscoverability(QLowEnergyAdvertisingData::DiscoverabilityGeneral);
  //advertisingData.setIncludePowerLevel(true);

  QList<QBluetoothUuid> serviceList;
  serviceList.append(QBluetoothUuid::DeviceInformation);
  advertisingData.setServices(serviceList);

  QByteArray manufacturerData;
  try
  {
    manufacturerData.append(QByteArray(productId, sizeof(productId)));
    manufacturerData.append(getDeviceSerialNumber());
  }
  catch (QString const & errorMessage)
  {
    QErrorMessage errorDialog;
    errorDialog.showMessage(errorMessage);
    errorDialog.exec();
    return;
  }

  advertisingData.setManufacturerData(static_cast<quint16>(0x07a3), manufacturerData);

  resetGattDB();

  QLowEnergyAdvertisingParameters advertisingParameters;
  advertisingParameters.setMode(QLowEnergyAdvertisingParameters::Mode::AdvInd);

  m_controller->startAdvertising(advertisingParameters, advertisingData);
  m_beaconOnOff->setText(defaults::BeaconOffButtonText);
  m_beaconRunning = true;
}

void SimulatorWindow::onControllerConnect()
{
  QString status = QString("Connected to %1")
    .arg(m_controller->remoteAddress().toString());

  m_statusBar->showMessage(status);
}

void SimulatorWindow::onControllerDisconnect()
{
  m_statusBar->showMessage("Disconnected");
  stopBeacon();
  startBeacon();
}

void SimulatorWindow::stopBeacon()
{
  m_controller->stopAdvertising();
  m_beaconOnOff->setText(defaults::BeaconOnButtonText);
  m_beaconRunning = false;
}

QLowEnergyServiceData buildXleSetupService()
{
  QLowEnergyServiceData serviceData;
  serviceData.setType(QLowEnergyServiceData::ServiceTypePrimary);
  serviceData.setUuid(xle_service::UuidXleService);
  serviceData.addCharacteristic(buildChar(xle_service::UuidQrCode));
  serviceData.addCharacteristic(buildChar(xle_service::UuidProvisionStatus));
  serviceData.addCharacteristic(buildChar(xle_service::UuidSimIccid));
  serviceData.addCharacteristic(buildChar(xle_service::UuidModemImei));
  serviceData.addCharacteristic(buildChar(xle_service::UuidCellularSignalStrength));
  serviceData.addCharacteristic(buildChar(xle_service::UuidMeshBackhaulType));
  serviceData.addCharacteristic(buildChar(xle_service::UuidWiFiBackhaulStats));
  return serviceData;
}

QLowEnergyServiceData buildDeviceInformationService()
{
  QLowEnergyServiceData serviceData;
  serviceData.setType(QLowEnergyServiceData::ServiceTypePrimary);
  serviceData.setUuid(QBluetoothUuid::DeviceInformation);
  serviceData.addCharacteristic(buildChar(device_information::UuidSystemId));
  serviceData.addCharacteristic(buildChar(device_information::UuidModelNumber));
  serviceData.addCharacteristic(buildChar(device_information::UuidSerialNumber));
  serviceData.addCharacteristic(buildChar(device_information::UuidFirmwareRevision));
  serviceData.addCharacteristic(buildChar(device_information::UuidHardwareRevision));
  serviceData.addCharacteristic(buildChar(device_information::UuidSoftwareRevision));
  serviceData.addCharacteristic(buildChar(device_information::UuidManufacturerName));
  return serviceData;
}

QLowEnergyCharacteristicData buildChar(
  QBluetoothUuid const & id,
  QLowEnergyCharacteristic::PropertyTypes const & props,
  QByteArray const & value)
{
  QLowEnergyCharacteristicData charInfo;
  charInfo.setUuid(id);
  charInfo.setProperties(props);
  if (value.length() > 0)
    charInfo.setValue(value);
  return charInfo;
}
