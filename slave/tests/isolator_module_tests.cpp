#include <glog/logging.h>

#include "isolator_module.cpp"
#include "mock_input_assigner.hpp"

using testing::Return;

TEST(IsolatorModuleTests, recover_smoke) {
  mesos::Parameters params;
  mesos::Parameter* param = params.add_parameter();
  param->set_key("test");
  param->set_value("value");

  std::shared_ptr<MockInputAssigner> mock_assigner(new MockInputAssigner());
  stats::IsolatorModule<MockInputAssigner> mod(mock_assigner);

  mesos::ExecutorInfo exec_info;
  exec_info.mutable_executor_id()->set_value("test executor");
  mesos::ContainerID container_id;
  container_id.set_value("test container");

  mesos::slave::ContainerState container_state;
  *container_state.mutable_executor_info() = exec_info;
  *container_state.mutable_container_id() = container_id;
  container_state.set_pid(123456);
  container_state.set_directory("test dir");
  std::list<mesos::slave::ContainerState> container_states;
  container_states.push_back(container_state);

  hashset<mesos::ContainerID> container_ids;
  container_ids.insert(container_id);

  EXPECT_CALL(*mock_assigner, register_containers(testing::_));
  mod.recover(container_states, container_ids).get();
}

TEST(IsolatorModuleTests, prepare_smoke) {
  mesos::Parameters params;
  mesos::Parameter* param = params.add_parameter();
  param->set_key("test");
  param->set_value("value");

  std::shared_ptr<MockInputAssigner> mock_assigner(new MockInputAssigner());
  stats::IsolatorModule<MockInputAssigner> mod(mock_assigner);

  mesos::ContainerID container_id;
  container_id.set_value("test container");

  mesos::slave::ContainerConfig config;
  config.mutable_executor_info()->mutable_executor_id()->set_value("test executor");
  config.set_directory("test directory");
  config.set_user("test user");

  EXPECT_CALL(*mock_assigner, register_container(container_id, config.executor_info()));
  Option<mesos::slave::ContainerLaunchInfo> ret =
    mod.prepare(container_id, config).get();
  EXPECT_TRUE(ret.isNone());
}

TEST(IsolatorModuleTests, cleanup_smoke) {
  mesos::Parameters params;
  mesos::Parameter* param = params.add_parameter();
  param->set_key("test");
  param->set_value("value");

  std::shared_ptr<MockInputAssigner> mock_assigner(new MockInputAssigner());
  stats::IsolatorModule<MockInputAssigner> mod(mock_assigner);

  mesos::ContainerID container_id;
  container_id.set_value("test container");

  EXPECT_CALL(*mock_assigner, unregister_container(container_id));
  mod.cleanup(container_id).get();
}

int main(int argc, char **argv) {
  ::google::InitGoogleLogging(argv[0]);
  FLAGS_logtostderr = 1;
  ::testing::InitGoogleTest(&argc, argv);
  process::initialize();
  return RUN_ALL_TESTS();
}
