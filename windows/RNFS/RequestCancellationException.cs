using System;

namespace RNFS
{
    class RequestCancellationException : Exception
    {
        protected int jobId;
        protected string filepath;

        public RequestCancellationException(int jobId, string filepath)
            : base("CANCELLED: job '" + jobId + "' to file '" + filepath + "'")
        {
            this.jobId = jobId;
            this.filepath = filepath;
        }

        public RequestCancellationException(int jobId, string filepath, Exception inner)
            : base("CANCELLED: job '" + jobId + "' to file '" + filepath + "'", inner)
        {
            this.jobId = jobId;
            this.filepath = filepath;
        }

        public RequestCancellationException(int jobId, string filepath, string message, Exception inner)
            : base(message, inner)
        {
            this.jobId = jobId;
            this.filepath = filepath;
        }

        public int getJobId()
        {
            return jobId;
        }

        public string getFilepath()
        {
            return filepath;
        }
    }
}
