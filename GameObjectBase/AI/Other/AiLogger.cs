using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AiLibrary.Other
{
    public interface IAiLogger
    {
        void Log(string message);
        void Warning(string message);
        void Error(string message);
    }

    public static class AiLogger
    {
        private static IAiLogger _instance;

        public static void SetLogger(IAiLogger logger) => _instance = logger;

        public static void Error(string message) => _instance?.Error(message);

        public static void Log(string message) => _instance?.Log(message);

        public static void Warning(string message) => _instance?.Warning(message);
    }
}
