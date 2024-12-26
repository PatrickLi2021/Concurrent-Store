Project 5A - Concurrent Store
====================

<!-- TODO: Fill this out. -->

## Design Overview:
**Step 1: Single-Threaded Simple KV Store**

For Step 1 of the project, I had to first define an interval key-value store, which I denoted as `std::map<std::string, std::string> kvmap`. The reason why I did this is because an std::map allows for rapid access of values when given a key and was the most suitable data structure for a KV store. Next, I had to implement the `get`, `put`, `append`, `delete`, `MultiGet`, `MultiPut`, and `AllKeys` functions. For these functions, I didn't have any special designs, I just followed the function headers to complete them.

**Step 2: Concurrent Simple KV Store**

For Step 2, I made the server able to handle multiple requests in parallel by adding synchronization primitives. To do this, I first added an `std::mutex mtx` to my header file. Afterwards, I utilized coarse-grained locking for the `put`, `append`, `delete`, `MultiGet`, and `MultiPut` methods by locking this mutex at the beginning of each function and unlocking at the end.

**Step 3: Synchronized Queue**

For Step 3, we were asked to implement a synchronized queue, which the server will use to concurrently handle multiple clients and send their requests to the simple key-value store. 

To do this, I used 2 synchronization fields (`std::mutex mtx` and `std::condition_variable cv`). I used a unique lock for the mutex in the size method because the size of the queue might have changed for one thread while another thread is getting the size. I use the condition variable in `pop`. For the rest of the functions, I mainly followed the header comments.

**Step 4: Single-Threaded Bucket-Based KV Store**

My main design approach for Step 4 was to conceptually understand that the hash table was divided into buckets that each contained some number of key-value pairs. I used the helper functions that we were provided to implement each of the hash table functions.

**Step 5: Concurrent Bucket-Based KV Store**

For Step 5, we needed to modify the bucket-based key value store implementation to support both safe and concurrent access. To do this, I used the `std::shared_mutex` and added this synchronization primitive to DbMap to synchronize access to the buckets. I also utilized an array of shared mutexes with one mutex for each bucket in the hash table. For `get`, `put`, `delete`, and `append`, I used the mutex associated with the relevant bucket index to lock access. I also used the exclusive lock for modifying operations like `delete`, `put`, and `append` and I used the shared lock for `get`.

In regards to making `MultiGet` and `MultiPut` atomic, I used the following approach:
- First, I loop over the keys to get the values of the bucket indices and store them inside a vector. I used the `bucket()` method for the `DBMap` store to do this.
- Next, I sort this vector of bucket indices and remove the duplicates. This is to enforce a hierarchical locking scheme.
- Then, I lock each bucket in this order and go in to access and modify the store.
- Lastly, unlock all the shared locks.

## Collaborators:
Aaron Wang

## Extra Credit Attempted:
N/A

## How long did it take to complete the Concurrent Store portion of KVStore?
8 hours

<!-- Enter an approximate number of hours that you spent actively working on the project. -->


Project 5B - Distributed Store
====================

## Design Overview:

### Overview of GDPRDelete Functionality:
My GDPRDelete function revolved around the idea of deleting all the posts associated with the input user ID. The first thing I do is retrieve a list of all post IDs via the Get method. Afterwards, I check if the list of post IDs is null. If it is, I return true immediately. If it is not, I then remove whitespace from the list of post IDs. Next, I iterate through this string, extract each post ID, and push it into a vector.

I iterate through this vector and delete the post content associated with each post ID. Lastly, I delete the user ID from all_users and delete the user ID to username key-value pair.

### Answers to README Questions:

#### Question 1:
My stakeholder pair was Pair #2 (Sarsra Breisand and Beth Abraham)

#### Question 2:
In this specific scenario, my first instinct was that I wanted to protect Breisand’s privacy as much as possible. As a result, I decided to delete the post made by Breisand on her account as well as her user ID from the list of user IDs. The most compelling reason and/or justification for this deletion was via the CCPA Right to Delete law, which states that users can request the deletion of any personal data at any time. Since Breisand made the request for her entire account (and the data associated with her account) to be deleted and this data is not integral to Tweeter and only serves as a distraction (and possibly a safety issue) for Breisand, it makes sense to delete this information. 

Another compelling ethical case for this type of deletion was via the GDPR Right to be Forgotten Law (Article 17) which states that “users can request the deletion of their personal data under six specific circumstances,” one of which is if the user revokes consent. Since Breisand is asking for her account to be deleted, she is thereby revoking consent for people to access her account information, and thus her posts and user ID.

#### Question 3:
However, one of the limitations/tradeoffs of this function is that it gets rid of posts that Abraham might deem useful for her research about the Breisand effect. The Breisand effect details how attempts to remove information might lead to unintentional increased awareness about that information to the general public. Getting rid of Breisand’s post that talks about removing her address will prevent other people from replying to this post and will limit Abraham’s ability to measure how big the Breisand effect becomes. However, I weighed Breisand’s privacy and right to be forgotten as more important, and this deletion method satisfies her request. In an attempt to partially satisfy Abraham’s point of view, I decided to leave the replies to Streisand’s post untouched, as these are not considered Streisand’s intellectual property and therefore shouldn’t be deleted. Thus, Abraham still has a few replies that she could possibly use to research the psychological phenomena of the Breisand effect.

Another related consequence of this deletion is that it doesn’t delete the actual post made by the paparazzi reporter that disclosed information about Breisand’s address. While Breisand may want this information to be deleted, I also deemed it not her intellectual property and thus, she has no control over it. 

One of the limitations of this function is that if Breisand had multiple posts on her account that don’t mention her address, it would delete those posts as well. For an arbitrary user utilizing this function, it would delete all posts made on their account, which may not be the intention in every deletion case. This was one of the limitations of the tools we were given for the GDPR delete function itself. Because the function only takes in a singular user ID as a parameter, it is difficult to make the delete function work using more granular criteria (i.e. key words in a post to delete). If more arguments could be added, I would consider adding a search term parameter so that only posts containing her address are deleted (in this case).

Some short-term stakeholders that could be adversely affected by this decision include members of Breisand’s family who live at that address or others in the area (as now there is probably unwanted attention by media and reporters as a result of this deletion). Others could include paparazzi reporters or other members of the media who could be reprimanded by spreading this information for publicity benefits. In the long-term, those who post things online about themselves (or about others) could face the same consequences and have unwanted attention attracted to them as a result of this event.

#### Question 4:
I think if I was asked to consider the interests of the paparazzi reporter, I might not have implemented a function that deletes all the posts on a particular account. This is because the paparazzi reporter may want to have that post remain in order to attract media attention to them. If the people who replied to Breisand’s post wanted their comments deleted as well, I would have changed my GDPR functionality to delete all the posts as well as the replies associated with each post. 

## Collaborators:
Angela Li

## Extra Credit Attempted:
N/A

## How long did it take to complete the Distributed Store portion of KVStore?
5

<!-- Enter an approximate number of hours that you spent actively working on the project. -->
