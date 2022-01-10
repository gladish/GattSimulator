
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QStatusBar>

#include <QSharedPointer>
#include <QtBluetooth/QLowEnergyController>
#include <QtBluetooth/QLowEnergyService>

class SimulatorWindow : public QMainWindow
{
public:
  SimulatorWindow(QWidget * parent = nullptr);

public slots:
  void quit();

private slots:
  void beaconOnOffReleased();
  void onControllerDisconnect();
  void onControllerConnect();

private:
  void startBeacon();
  void stopBeacon();
  void resetGattDB();
  void writeDeviceInformation(QBluetoothUuid const & characteristicId, QByteArray const & value);
  void writeXleInformation(QBluetoothUuid const & characteristicId, QByteArray const & value);

  QByteArray getDeviceSerialNumber() const;

  void updateGattSystemId();
  void updateGattModelNumber();
  void updateGattSerialNumber();
  void updateGattFirmwareRevision();
  void updateGattHardwareRevision();
  void updateGattSoftwareRevision();
  void updateGattManufacturerName();

  void updateGattQrCode();
  void updateGattProvisionStatus();
  void updateGattSimIccid();
  void updateGattModemImei();
  void updateGattCellularSignalStrength();
  void updateGattMeshBackhaulType();
  void updateGattWiFiBackhaulStats();

private:
  // deviceinfo service
  QLineEdit * m_systemId;
  QLineEdit * m_modelNumber;
  QLineEdit * m_serialNumber;
  QLineEdit * m_firmwareRevision;
  QLineEdit * m_hardwareRevision;
  QLineEdit * m_softwareRevision;
  QLineEdit * m_manufacturerName;

  // xle service values
  QLineEdit * m_qrCode;
  QLineEdit * m_provisionStatus;
  QLineEdit * m_simIccid;
  QLineEdit * m_modemImei;
  QLineEdit * m_cellularSignalStrength;
  QLineEdit * m_meshBackhaulType;
  QLineEdit * m_wifiBackhaulStats;

  QPushButton * m_beaconOnOff;
  bool m_beaconRunning;

  QSharedPointer<QLowEnergyController> m_controller;
  QSharedPointer<QLowEnergyService> m_setupService;
  QSharedPointer<QLowEnergyService> m_deviceInfoService;

  QStatusBar * m_statusBar;
};
