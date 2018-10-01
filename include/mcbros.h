#ifndef MCBROS_H
#define MCBROS_H

#include <QObject>
#include <string>
#include <QString>
#include <stdint.h>
#include <QVector>
//#include <vector>
#include "medlab_motor_control_board/McbEncoders.h"
#include "medlab_motor_control_board/McbEncoderCurrent.h"
#include "medlab_motor_control_board/McbStatus.h"
#include "medlab_motor_control_board/EnableMotor.h"
#include "medlab_motor_control_board/McbGains.h"
#include "ros/ros.h"

Q_DECLARE_METATYPE(std::vector<int32_t>)
Q_DECLARE_METATYPE(medlab_motor_control_board::McbEncoderCurrent)

class McbRos : public QObject
{
  Q_OBJECT

public:
  McbRos();
  ~McbRos();
  bool init(std::string nodeName);
  bool isConnected(void);
  bool enableRosControl(bool cmd);
  bool enableAllMotors(bool cmd);
  bool enableMotor(uint8_t motor, bool cmd);
  bool setDesiredPosition(medlab_motor_control_board::McbEncoders desiredPositions);
  bool setDesiredPosition(int motor, qint32 position);
  QVector<float> getEfforts(void);
  bool zeroCurrentPosition(uint8_t motor); // zeroes encoder count of a single motor
  bool zeroCurrentPositions(void); // zeroes encoder count for all motors
  bool setPid(uint8_t motor, float p, float i, float d);
  int  getNumMotors(void);
  QString getIp(void);   // returns IP address via uint8_t[4]
  QString getMac(void); // returns MAC address via uint8_t[6]
  bool isRosControlEnabled(void); // NOTE: may not be accurate if state has changed since last status query
  bool isMotorEnabled(uint8_t motor);

public slots:
  bool requestStatus(void);

private:
  std::string nodeName_;
  ros::NodeHandle nh_;
  ros::Publisher  pubEnableRos_;
  ros::Publisher  pubEnableAllMotors_;
  ros::Publisher  pubEnableMotor_;
  ros::Publisher  pubGetStatus_;
  ros::Publisher  pubEncoderCommand_;
  ros::Publisher  pubZeroSingle_;
  ros::Publisher  pubZeroAll_;
  ros::Publisher  pubSetGains_;
  ros::Subscriber subStatus_;
  ros::Subscriber subEncoderCurrent_;
  ros::Subscriber limitSwitchEvent_;

  bool connected_ = false; // true after first status message received
  bool currentControlState_; // 0 => ROS Idle; 1 => ROS Control
  medlab_motor_control_board::McbStatus currentStatus_; // most recently received status
  medlab_motor_control_board::McbEncoderCurrent encoderCurrent_; // measured and desired positions

  void callbackSubEncoderCurrent(const medlab_motor_control_board::McbEncoderCurrent::ConstPtr& msg);
  void callbackSubStatus(const medlab_motor_control_board::McbStatus::ConstPtr& msg);

signals:
  void connectionEstablished(void);
  void connectionLost(void);
  void newPositions(medlab_motor_control_board::McbEncoderCurrent);
  void controlStateChanged(bool); // 0 => ROS Idle; 1 => ROS Control
  void motorStateChanged(int); // index of motor that changed
  void newStatus(void); // emitted after every new status is received
  void lastMotorStates(QVector<bool>);
};

#endif // MCBROS_H