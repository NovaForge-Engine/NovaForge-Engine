using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace InteractableGroupsAi.Agents
{
    public interface IPerceptionSensor
    {
        Action<IAgentState> OnAgentDetected { get; set; }
        Action<IAgentState> OnAgentLost { get; set; }
        Action<IAgentState> OnAgentMoved { get; set; }

        void Update();
    }
}
