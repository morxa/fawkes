
/***************************************************************************
 *  webview-agent-processor.h - Process agent information from the blackboard
 *
 *  Created: Wed Oct 22 20:21:42 2014
 *  Copyright  2014  Till Hofmann
 ****************************************************************************/

/*  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  Read the full text in the LICENSE.GPL file in the doc directory.
 */

#include "webview-agent-processor.h"

#include <core/exception.h>
#include <core/threading/mutex_locker.h>
#include <logging/logger.h>
#include <webview/page_reply.h>
#include <webview/error_reply.h>
#include <webview/file_reply.h>
#include <webview/redirect_reply.h>
#include <blackboard/blackboard.h>
#include <interfaces/AgentInterface.h>

#include <sstream>

#include <cstring>
#include <cerrno>

#include <gvc.h>
#include <gvcjob.h>

using namespace fawkes;
using namespace std;

/** @class WebviewAgentRequestProcessor "webview-agent-processor.h"
 * Agent request processor.
 * @author Till Hofmann
 */


/** Constructor.
 * @param base_url base URL of the webview agent web request processor.
 * @param agent_id AgentInterface ID
 * @param blackboard blackboard to open interfaces 
 * @param logger logger to report problems
 */
WebviewAgentRequestProcessor::WebviewAgentRequestProcessor(
  string base_url, string agent_id,
  fawkes::BlackBoard *blackboard, fawkes::Logger *logger)
: baseurl_(base_url), blackboard_(blackboard), logger_(logger)
{
  agent_if_       = blackboard->open_for_reading<AgentInterface>(agent_id.c_str());
}


/** Destructor. */
WebviewAgentRequestProcessor::~WebviewAgentRequestProcessor()
{
  blackboard_->close(agent_if_);
}


WebReply *
WebviewAgentRequestProcessor::process_request(const fawkes::WebRequest *request)
{
  if (request->url().compare(0, baseurl_.length(), baseurl_) == 0) {
    // It is in our URL prefix range
    string subpath = request->url().substr(baseurl_.length());

    if (subpath == "/graph.png") {
      string graph = generate_graph_string();

      logger_->log_debug("WebviewAgentProcessor: ", "graph string is %s", graph.c_str());
      FILE *f = tmpfile();
      if (NULL == f) {
        return new WebErrorPageReply(WebReply::HTTP_INTERNAL_SERVER_ERROR,
            "Cannot open temp file: %s", strerror(errno));
      }

      string_to_graph(graph, f);

      try {
        DynamicFileWebReply *freply = new DynamicFileWebReply(f);
        return freply;
      } catch (fawkes::Exception &e) {
        return new WebErrorPageReply(WebReply::HTTP_INTERNAL_SERVER_ERROR, *(e.begin()));
      }

    } else {
      WebPageReply *r = new WebPageReply("Agent");
      r->append_body("<p><img src=\"%s/graph.png\" /></p>", baseurl_.c_str());
      //      r->append_body("<pre>%s</pre>", graph.c_str());
      return r;
    }
  } else {
    return NULL;
  }
}

void
WebviewAgentRequestProcessor::string_to_graph(string graph, FILE * output)
{
  GVC_t* gvc = gvContext(); 
  Agraph_t* G = agmemread((char *)graph.c_str());
  gvLayout(gvc, G, (char *)"dot");
  gvRender(gvc, G, "png", output);
  gvFreeLayout(gvc, G);
  agclose(G);    
  gvFreeContext(gvc);
}

string
WebviewAgentRequestProcessor::generate_graph_string()
{
  stringstream gstream;
  agent_if_->read();
  gstream << "digraph { graph [fontsize=14];";
  gstream << "node [fontsize=12]; edge [fontsize=12]; " << endl;

  if (! agent_if_->has_writer()) {
    gstream << "\"No writer for agent interface. No agent running\"";
    gstream << "}";
    return gstream.str();
  }

  string history = agent_if_->history();
  string delimiter = ";";
  size_t last_match_pos = 0;
  while (size_t match_pos = history.find(delimiter, last_match_pos) && match_pos != string::npos) {
    string action = history.substr(last_match_pos, match_pos - last_match_pos);
    if (last_match_pos != 0) {
      gstream << " -> ";
    }
    gstream << '"' << action << '"';
    last_match_pos = match_pos;
  }
  return "";
}

  

    
    
