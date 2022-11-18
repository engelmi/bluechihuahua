#ifndef BLUECHIHUAHUA_TYPES_ENUM
#define BLUECHIHUAHUA_TYPES_ENUM

typedef enum JobType JobType;
typedef enum NodeJobType NodeJobType;
typedef enum JobState JobState;
typedef enum JobResult JobResult;

enum JobType { JOB_ISOLATE_ALL, _JOB_TYPE_MAX, _JOB_TYPE_INVALID = -1 };

enum NodeJobType {
	NODE_JOB_ISOLATE,
	_NODE_JOB_TYPE_MAX,
	_NODE_JOB_TYPE_INVALID = -1
};

enum JobState {
	JOB_WAITING,
	JOB_RUNNING,
	_JOB_STATE_MAX,
	_JOB_STATE_INVALID = -1
};

enum JobResult {
	JOB_DONE,
	JOB_CANCELED,
	JOB_FAILED,
	_JOB_RESULT_MAX,
	_JOB_RESULT_INVALID = -1
};

extern const char *job_type_to_string(JobType type);
extern const char *node_job_type_to_string(NodeJobType type);
extern const char *job_state_to_string(JobState state);
extern const char *job_result_to_string(JobResult result);

#endif